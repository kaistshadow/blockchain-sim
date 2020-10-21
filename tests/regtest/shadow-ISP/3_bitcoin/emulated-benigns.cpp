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

typedef unsigned int SOCKET;
#define INVALID_SOCKET      (SOCKET)(~0)


std::string string_to_hex(const std::string& input)
{
    static const char hex_digits[] = "0123456789ABCDEF";

    std::string output;
    output.reserve(input.length() * 2);
    for (unsigned char c : input)
    {
        output.push_back(hex_digits[c >> 4]);
        output.push_back(hex_digits[c & 15]);
    }
    return output;
}
// Convert string of hex numbers to its equivalent char-stream
std::string hex_to_string(const std::string& hexstr)
{
    std::string output;
    output.resize((hexstr.size() + 1) / 2);

    for (size_t i = 0, j = 0; i < output.size(); i++, j++)
    {
        output[i] = (hexstr[j] & '@' ? hexstr[j] + 9 : hexstr[j]) << 4, j++;
        output[i] |= (hexstr[j] & '@' ? hexstr[j] + 9 : hexstr[j]) & 0xF;
    }
    return output;
}

// bitcoin specific interface for receiving data.
// Read a 'nBytes' data from datastream ('pch'),
// then push back to message queues ('vRecvMsg' and 'vProcessMsg').
// Almost code are borrowed from net.cpp of Bitcoin repo.
bool BitcoinReceiveMsg(const char *pch, unsigned int nBytes, std::list<CNetMessage>& vRecvMsg, std::list<CNetMessage>& vProcessMsg) {
    while (nBytes > 0) {
        // get current incomplete message, or create a new one
        if (vRecvMsg.empty() || vRecvMsg.back().complete())
            vRecvMsg.push_back(CNetMessage(Params().MessageStart(), SER_NETWORK, INIT_PROTO_VERSION));

        CNetMessage& msg = vRecvMsg.back();

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

    return true;
}

// bitcoin specific interface for processing messages.
// Read and process a message 'msg',
// then push back any necessary replies to message queue ('vSendMsg').
// Almost code are borrowed from net_processing.cpp of Bitcoin repo.
bool BitcoinProcessMsg(CNetMessage& msg, std::deque<std::vector<unsigned char>>& vSendMsg, std::string their_ip, uint16_t their_port) {

    const unsigned char MessageStartChars[4] = {'\v', '\021', '\t', '\a'};

    if (memcmp(msg.hdr.pchMessageStart, MessageStartChars, CMessageHeader::MESSAGE_START_SIZE) != 0) {
        std::cout << "INVALID MESSAGESTART " << msg.hdr.GetCommand() << "\n";
        exit(-1);
    }

    // their_addr
    CAddress their_addr;
    struct 	sockaddr_in 	new_addr;    /* my address information */
    new_addr.sin_family = AF_INET;         /* host byte order */
    new_addr.sin_port = htons(their_port);     /* short, network byte order */
    new_addr.sin_addr.s_addr = inet_addr(their_ip.c_str());
    bzero(&(new_addr.sin_zero), 8);        /* zero the rest of the struct */

    if (!their_addr.SetSockAddr((const struct sockaddr*)&new_addr)) {
        LogPrintf("Warning: Unknown socket family\n");
    }

    // Read header
    CMessageHeader& hdr = msg.hdr;
    if (!hdr.IsValid(MessageStartChars))
    {
        LogPrint(BCLog::NET, "PROCESSMESSAGE: ERRORS IN HEADER %s\n", hdr.GetCommand());
        exit(-1);
    }
    std::string strCommand = hdr.GetCommand();
    CDataStream& vRecv = msg.vRecv;

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

        // version message
        ServiceFlags nLocalNodeServices = ServiceFlags(NODE_NETWORK|NODE_WITNESS|NODE_NETWORK_LIMITED);
        uint64_t nonce = 0;
        int myNodeStartingHeight = nStartingHeight;

        CNetMsgMaker(INIT_PROTO_VERSION).Make(NetMsgType::VERSION, PROTOCOL_VERSION, (uint64_t)nLocalNodeServices, nTime, their_addr, addrFrom, nonce, strSubVersion, myNodeStartingHeight, true);
        CSerializedNetMsg msg = CNetMsgMaker(INIT_PROTO_VERSION).Make(NetMsgType::VERSION, PROTOCOL_VERSION, (uint64_t)nLocalNodeServices, nTime, their_addr, addrFrom, nonce, strSubVersion, myNodeStartingHeight, true);

        size_t nMessageSize = msg.data.size();
        size_t nTotalSize = nMessageSize + CMessageHeader::HEADER_SIZE;
        LogPrint(BCLog::NET, "sending %s (%d bytes) \n",  SanitizeString(msg.command.c_str()), nMessageSize);

        std::vector<unsigned char> serializedHeader;
        serializedHeader.reserve(CMessageHeader::HEADER_SIZE);
        uint256 hash = Hash(msg.data.data(), msg.data.data() + nMessageSize);
        CMessageHeader hdr(MessageStartChars, msg.command.c_str(), nMessageSize);
        memcpy(hdr.pchChecksum, hash.begin(), CMessageHeader::CHECKSUM_SIZE);

        CVectorWriter{SER_NETWORK, INIT_PROTO_VERSION, serializedHeader, 0, hdr};

        if (nMessageSize) {
            vSendMsg.push_back(std::move(serializedHeader));
            vSendMsg.push_back(std::move(msg.data));
        }
    }
    return true;
}



template<typename MSG>
class SocketControlStruct {
private:
    SOCKET _socketfd;
    size_t _offset;
    ev::io _datasocket_watcher;
    std::list<MSG> vRecvMsg;
    std::list<MSG> vProcessMsg;
    std::deque<std::vector<unsigned char>> vSendMsg;

public:
    std::string their_ip;
    uint16_t their_port;

    SocketControlStruct() {}
    SocketControlStruct(int fd, std::string ip, uint16_t port) {_offset = 0; _socketfd = fd; their_ip = ip, their_port = port;}
    size_t getSendOffset() {return _offset;}
    void setSendOffset(size_t offset) { _offset = offset; }
    std::list<MSG>& getVRecvMsg() {return vRecvMsg;}
    std::list<MSG>& getVProcessMsg() {return vProcessMsg;}
    std::deque<std::vector<unsigned char>>& getVSendMsg() {   return vSendMsg;  }
    ev::io& getSocketWatcher() {return _datasocket_watcher;}

    // Copy constructor : it is needed due to rvalue(?) assignment for std::map. Don't copy a watcher.
    // TODO : do not use copy. but use only move because watcher cannot be copied(?).
    SocketControlStruct(const SocketControlStruct &s2) {_socketfd = s2._socketfd; _offset = s2._offset; vRecvMsg = s2.vRecvMsg; vProcessMsg = s2.vProcessMsg; vSendMsg = s2.vSendMsg; their_ip = s2.their_ip; their_port = s2.their_port;}

};




template<typename MSG,
        bool (*ReceiveMSG)(const char *, unsigned int, std::list<MSG>&, std::list<MSG>&),
        bool (*ProcessMSG)(MSG& , std::deque<std::vector<unsigned char>>&, std::string, uint16_t) >
class PassiveNode {
private:
    ev::io _listen_watcher; // assume a single listening socket per Node
    void _listenSocketIOCallback (ev::io &w, int revents) {
        std::cout << "listen socket IO callback called!" << "\n";

        struct 	sockaddr_in 	their_addr; /* connector's address information */
        int sock_fd;
        socklen_t 			sin_size;

        sin_size = sizeof(struct sockaddr_in);
        sock_fd = accept(w.fd, (struct sockaddr *)&their_addr, &sin_size);
        if ( sock_fd != -1 ) {
            fcntl(sock_fd, F_SETFL, O_NONBLOCK);

            // Create DataSocket and start a event watcher for the DataSocket
            mSocketControl.try_emplace(sock_fd, SocketControlStruct<MSG>(sock_fd, inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port)));
            std::cout << "server: got connection from " << mSocketControl[sock_fd].their_ip << ":" << mSocketControl[sock_fd].their_port << "\n";

            ev::io& watcher = mSocketControl[sock_fd].getSocketWatcher();
            watcher.set<PassiveNode, &PassiveNode::_dataSocketIOCallback> (this);
            watcher.start(sock_fd, ev::READ);
        }
        else {
            if( errno != EAGAIN && errno != EWOULDBLOCK ) {
                std::cout << "accept() failed errno=" << errno << strerror(errno) << "\n";
                exit(-1);
            }
        }
    }

    void _dataSocketIOCallback (ev::io &w, int revents) {
        std::cout << "data socket IO callback called!" << "\n";

        int datasock_fd = w.fd;
        auto& socketControl = mSocketControl[datasock_fd];
        std::list<MSG>& vRecvMsg = socketControl.getVRecvMsg();
        std::list<MSG>& vProcessMsg = socketControl.getVProcessMsg();
        std::deque<std::vector<unsigned char>>& vSendMsg = socketControl.getVSendMsg();

        if (revents & EV_READ) {
            std::cout << "DoRecv!" << "\n";

            // recv from socket, and append received data into the buffer.
            // typical socket buffer is 8K-64K
            char pchBuf[0x10000];

            int nBytes = 0;

            nBytes = recv(datasock_fd, pchBuf, sizeof(pchBuf), MSG_DONTWAIT); // What is the meaning of MSG_DONTWAIT?
            if (nBytes > 0) {
                std::string recv_str(pchBuf, nBytes);
                std::cout << "received data :[" << recv_str << "]" << "\n";
                std::cout << "received data hex:[" << string_to_hex(recv_str) << "]" << "\n";

                bool ret = ReceiveMSG(pchBuf, nBytes, vRecvMsg, vProcessMsg);
                if (!ret) {
                    std::cout << "error while BitcoinReceiveMsg" << "\n";
                    exit(-1);
                }

                if (!vProcessMsg.empty()) {
                    std::list<MSG> msgs;

                    // Just take one message
                    msgs.splice(msgs.begin(), vProcessMsg, vProcessMsg.begin());
                    MSG& msg(msgs.front());

                    std::cout << msg.hdr.GetCommand() << "\n";

                    // process message
                    ret = ProcessMSG(msg, vSendMsg, socketControl.their_ip, socketControl.their_port);
                    if (!ret)
                    {
                        std::cout << "error while processing message" << "\n";
                        exit(-1);
                    }
                    if (!vSendMsg.empty()) // set Writable
                        socketControl.getSocketWatcher().set(datasock_fd, ev::READ | ev::WRITE);
                }

            } else if (nBytes == 0) {
                std::cout << "connection closed while recv" << "\n";
                delete this; // is it okay for this self-destruction?
            } else if (nBytes < 0) {
                // error
                std::cout << "Error while recv" << "\n";
            }
        }
        else if (revents & EV_WRITE) {
            auto it = vSendMsg.begin();
            size_t nSentSize = 0;

            size_t nSendOffset = socketControl.getSendOffset();

            while (it != vSendMsg.end()) {
                const auto &data = *it;
                assert(data.size() > nSendOffset);
                int nBytes = 0;
                {
                    nBytes = send(datasock_fd, reinterpret_cast<const char*>(data.data()) + nSendOffset, data.size() - nSendOffset, MSG_NOSIGNAL | MSG_DONTWAIT);
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
                        if (nErr != WSAEWOULDBLOCK && nErr != WSAEMSGSIZE && nErr != WSAEINTR && nErr != WSAEINPROGRESS)
                        {
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
                socketControl.getSocketWatcher().set(datasock_fd, ev::READ);
            }
            vSendMsg.erase(vSendMsg.begin(), it);
        }
    }



private:
    int _listen_sockfd;
    std::string _shadow_ip;

    // data structures
    std::map< SOCKET, SocketControlStruct<MSG> > mSocketControl;


    bool SetSocketNonBlocking(const SOCKET& hSocket, bool fNonBlocking)
    {
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

    bool CloseSocket(SOCKET& hSocket)
    {
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
        setsockopt(hSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&set, sizeof(int));

        // Set the non-blocking option on the socket.
        if (!SetSocketNonBlocking(hSocket, true)) {
            CloseSocket(hSocket);
            std::cout << "Setting socket to non-blocking failed" << "\n";
            exit(-1);
        }

        return hSocket;
    }
public:
    PassiveNode(std::string ip_addr, int port) {
        _shadow_ip = ip_addr;
        std::cout << "Node generated" << "\n";

        int sockfd = CreateNewSocket();

        // bind to shadow's virtual NIC
        struct 	sockaddr_in 	new_addr;    /* my address information */
        new_addr.sin_family = AF_INET;         /* host byte order */
        new_addr.sin_port = htons(port);     /* short, network byte order */
        new_addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
        bzero(&(new_addr.sin_zero), 8);        /* zero the rest of the struct */

        if (shadow_bind(sockfd, (struct sockaddr *)&new_addr, sizeof(struct sockaddr)) == -1) {
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
        _listen_watcher.set<PassiveNode, &PassiveNode::_listenSocketIOCallback> (this);
        _listen_watcher.start(_listen_sockfd, ev::READ);
    }

};


int main(int argc, char *argv[]) {

    TestHelloBitcoinLib();

    exported_main();

    std::cout << "Starting ISP-server for emulated benign nodes" << "\n";

    puts_temp("test shadow_interface\n");

    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> benign_node1("1.1.0.1", 18333);
    PassiveNode<CNetMessage, BitcoinReceiveMsg, BitcoinProcessMsg> benign_node2("1.2.0.1", 18333);

    struct ev_loop *libev_loop = EV_DEFAULT;
    // ListenSocketWatcher listenSocketWatcher("1.2.0.1");

    //ListenSocketWatcher listenSocketWatcher2("11.0.0.11");

    while (true) {
        std::cout << "before ev_run" << "\n";
        ev_run (libev_loop, EVRUN_ONCE);
    }
}
