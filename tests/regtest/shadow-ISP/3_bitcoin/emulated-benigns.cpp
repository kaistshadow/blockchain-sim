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
#include <net_processing.h>

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

template<typename MSG, bool (*ReceiveMSG)(const char *, unsigned int, std::list<MSG>&, std::list<MSG>&)>
class DataSocket {
private:
    ev::io _datasocket_watcher;
    size_t _readoffset;
    std::list<MSG> vRecvMsg;
    std::list<MSG> vProcessMsg;

    int _socketfd;
    void _dataSocketIOCallback (ev::io &w, int revents) {
        std::cout << "data socket IO callback called!" << "\n";

        if (revents & EV_READ) {
            std::cout << "DoRecv!" << "\n";

            // recv from socket, and append received data into the buffer.
            // typical socket buffer is 8K-64K
            char pchBuf[0x10000];

            int nBytes = 0;

            nBytes = recv(_socketfd, pchBuf, sizeof(pchBuf), MSG_DONTWAIT); // What is the meaning of MSG_DONTWAIT?
            if (nBytes > 0) {
                std::string recv_str(pchBuf, nBytes);
                std::cout << "received data :[" << recv_str << "]" << "\n";
                std::cout << "received data hex:[" << string_to_hex(recv_str) << "]" << "\n";

                bool ret = BitcoinLibReceiveMsg(pchBuf, nBytes, vRecvMsg, vProcessMsg);
                if (!ret) {
                    std::cout << "error while BitcoinLibReceiveMsg" << "\n";
                    exit(-1);
                }

                if (!vProcessMsg.empty()) {
                    std::list<MSG> msgs;

                    // Just take one message
                    msgs.splice(msgs.begin(), vProcessMsg, vProcessMsg.begin());
                    MSG& msg(msgs.front());

                    std::cout << msg.hdr.GetCommand() << "\n";

                    // process message
//                    const unsigned char MessageStartChars[4] = {'\v', '\021', '\t', '\a'};
//
//                    if (memcmp(msg.hdr.pchMessageStart, MessageStartChars, CMessageHeader::MESSAGE_START_SIZE) != 0) {
//                        std::cout << "INVALID MESSAGESTART " << msg.hdr.GetCommand() << "\n";
//                        exit(-1);
//                    }
//
//                    // Read header
//                    CMessageHeader& hdr = msg.hdr;
//                    if (!hdr.IsValid(MessageStartChars))
//                    {
//                        LogPrint(BCLog::NET, "PROCESSMESSAGE: ERRORS IN HEADER %s\n", hdr.GetCommand());
//                        exit(-1);
//                    }
//
//                    ret = BitcoinLibProcessMessage(msg, MessageStartChars, sendMsgManager.GetSendMsg(_fd), sendMsgManager.GetAddr(_fd));
//                    if (!ret)
//                    {
//                        std::cout << "error while processing message" << "\n";
//                        exit(-1);
//                    }
//                    if (!sendMsgManager.GetSendMsg(_fd).empty())
//                        SetWritable(); // make watcher to monitor write event
                }

            } else if (nBytes == 0) {
                std::cout << "connection closed while recv" << "\n";
                delete this; // is it okay for this self-destruction?
            } else if (nBytes < 0) {
                // error
                std::cout << "Error while recv" << "\n";
            }
        }
    }
public:
    DataSocket() {}
    DataSocket(int fd) {
        _socketfd = fd;
        _readoffset = 0;
    }
    // Copy constructor : don't copy a watcher
    DataSocket(const DataSocket &s2) {_socketfd = s2._socketfd; _readoffset = s2._readoffset; vRecvMsg = s2.vRecvMsg; vProcessMsg = s2.vProcessMsg;}

    void StartWatcher() {
        _datasocket_watcher.set<DataSocket, &DataSocket::_dataSocketIOCallback> (this);
        _datasocket_watcher.start(_socketfd, ev::READ);
    }
};

//template<typename MSG>
//class RecvControl {
//private:
//    size_t _offset;
//    std::list<MSG> vRecvMsg;
//    std::list<MSG> vProcessMsg;
//public:
//    RecvControl() {_offset = 0;}
//    std::list<MSG>& getVRecvMsg() {return vRecvMsg;}
//    std::list<MSG>& getVProcessMsg() {return vProcessMsg;}
//};

template<typename MSG, bool (*ReceiveMSG)(const char *, unsigned int, std::list<MSG>&, std::list<MSG>&)>
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
            std::cout << "server: got connection from " << inet_ntoa(their_addr.sin_addr) << "\n";
            fcntl(sock_fd, F_SETFL, O_NONBLOCK);

            // Create DataSocket and start a event watcher for the DataSocket
            vDataSocket.push_back(DataSocket<MSG, ReceiveMSG>(sock_fd));
            vDataSocket.back().StartWatcher();
        }
        else {
            if( errno != EAGAIN && errno != EWOULDBLOCK ) {
                std::cout << "accept() failed errno=" << errno << strerror(errno) << "\n";
                exit(-1);
            }
        }
    }

private:
    int _listen_sockfd;
    std::string _shadow_ip;

    // data structures
    std::list<DataSocket<MSG, ReceiveMSG> > vDataSocket; // TODO: think about 'call by reference' to reduce unnecessary copying


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

    void testRecvMsg(int a) {
//        MSG msg;
////        msg.content = "test";
//        (*ReceiveMSG)(msg, a);
    }
};


int main(int argc, char *argv[]) {

    TestHelloBitcoinLib();

    exported_main();

    std::cout << "Starting ISP-server for emulated benign nodes" << "\n";

    puts_temp("test shadow_interface\n");

    PassiveNode<CNetMessage, BitcoinLibReceiveMsg> n("1.2.0.1", 18333);
    n.testRecvMsg(10);

    struct ev_loop *libev_loop = EV_DEFAULT;
    // ListenSocketWatcher listenSocketWatcher("1.2.0.1");

    //ListenSocketWatcher listenSocketWatcher2("11.0.0.11");

    while (true) {
        std::cout << "before ev_run" << "\n";
        ev_run (libev_loop, EVRUN_ONCE);
    }
}
