//
// Created by ilios on 20. 10. 28..
//

#include <ev++.h>

#include <algorithm>
#include <string_view>
#include <iostream>
#include <deque>
#include <list>
#include <vector>
#include <map>

#include "shadow_interface.h"
#include <arpa/inet.h>
#include <fcntl.h> /* Added for the nonblocking socket */
#include <memory>
#include <netinet/tcp.h>
#include <assert.h>

typedef unsigned int SOCKET;
#define INVALID_SOCKET      (SOCKET)(~0)
#define SOCKET_ERROR        -1

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

struct Message {
    int payload_size;
    std::string payload;
};

bool ReceiveMsg(const char *pch, unsigned int nBytes, std::list<Message>& vRecvMsg, std::list<Message>& vProcessMsg) {
    if (nBytes >= 4) {

        Message msg;
        memcpy(&msg.payload_size, pch, 4);
        assert(nBytes == 4 + msg.payload_size);
        msg.payload = std::string(pch+4, msg.payload_size);

        vProcessMsg.push_back(msg);
    }
    return true;
}

bool ProcessMsg(Message& msg, bool fromInbound, std::deque<std::vector<unsigned char>>& vSendMsg, std::string their_ip, uint16_t their_port) {

    if (msg.payload == "hello") {
        Message reply;
        reply.payload = "nice to meet you";
        reply.payload_size = reply.payload.size();

        unsigned char size[4];
        memcpy(size, &reply.payload_size, 4);

        std::vector<unsigned char> stream;
        stream.push_back(size[0]);
        stream.push_back(size[1]);
        stream.push_back(size[2]);
        stream.push_back(size[3]);

        stream.insert(std::end(stream), std::begin(reply.payload), std::end(reply.payload));

        vSendMsg.push_back(stream);
    }
    return true;
}

template<typename MSG>
class SocketControlStruct {
private:
    SOCKET _socketfd;
    size_t _offset;
    bool fInbound;
    ev::io _connsocket_watcher;
    ev::io _datasocket_watcher;
    std::list<MSG> vRecvMsg;
    std::list<MSG> vProcessMsg;
    std::deque<std::vector<unsigned char>> vSendMsg;

public:
    std::string their_ip;
    uint16_t their_port;

    SocketControlStruct() {}
    SocketControlStruct(int fd, std::string ip, uint16_t port, bool inbound = true) {_offset = 0; _socketfd = fd; their_ip = ip, their_port = port; fInbound = inbound;}
    size_t getSendOffset() {return _offset;}
    void setSendOffset(size_t offset) { _offset = offset; }
    std::list<MSG>& getVRecvMsg() {return vRecvMsg;}
    std::list<MSG>& getVProcessMsg() {return vProcessMsg;}
    std::deque<std::vector<unsigned char>>& getVSendMsg() {   return vSendMsg;  }
    ev::io& getDataSocketWatcher() {return _datasocket_watcher;}
    ev::io& getConnSocketWatcher() {return _connsocket_watcher;}
    bool isInboundSocket() { return fInbound; }

    // Copy constructor : it is needed due to rvalue(?) assignment for std::map. Don't copy a watcher.
    // TODO : do not use copy. but use only move because watcher cannot be copied(?).
    SocketControlStruct(const SocketControlStruct &s2) {_socketfd = s2._socketfd; _offset = s2._offset; fInbound = s2.fInbound; vRecvMsg = s2.vRecvMsg; vProcessMsg = s2.vProcessMsg; vSendMsg = s2.vSendMsg; their_ip = s2.their_ip; their_port = s2.their_port;}

};

template<typename MSG,
        bool (*ReceiveMSG)(const char *, unsigned int, std::list<MSG>&, std::list<MSG>&),
        bool (*ProcessMSG)(MSG& , bool, std::deque<std::vector<unsigned char>>&, std::string, uint16_t)>
class ServerNode {
private:
    ev::io _listen_watcher; // assume a single listening socket per Node
    ev::io _data_watcher; // assume a single data socket per Node
    // data structures
    std::map< SOCKET, SocketControlStruct<MSG> > mSocketControl;

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

            ev::io& watcher = mSocketControl[sock_fd].getDataSocketWatcher();
            watcher.set<ServerNode, &ServerNode::_dataSocketIOCallback> (this);
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
                std::cout << "received data(" << _shadow_ip << "):[" << recv_str << "]" << "\n";
                std::cout << "received data(" << _shadow_ip << ") hex:[" << string_to_hex(recv_str) << "]" << "\n";

                bool ret = ReceiveMSG(pchBuf, nBytes, vRecvMsg, vProcessMsg);
                if (!ret) {
                    std::cout << "error while ReceiveMSG" << "\n";
                    exit(-1);
                }

                if (!vProcessMsg.empty()) {
                    std::list<MSG> msgs;

                    // Just take one message
                    msgs.splice(msgs.begin(), vProcessMsg, vProcessMsg.begin());
                    MSG& msg(msgs.front());

                    // process message
                    ret = ProcessMSG(msg, socketControl.isInboundSocket(), vSendMsg, socketControl.their_ip, socketControl.their_port);
                    if (!ret)
                    {
                        std::cout << "error while processing message" << "\n";
                        exit(-1);
                    }
                    if (!vSendMsg.empty()) // set Writable
                        socketControl.getDataSocketWatcher().set(datasock_fd, ev::READ | ev::WRITE);
                }
            } else if (nBytes == 0) {
                std::cout << "connection closed while recv" << "\n";
                delete this; // is it okay for this self-destruction? // TODO : it's not okay for multiple data sockets
            } else if (nBytes < 0) {
                // error
                std::cout << "Error while recv" << "\n";
            }
        }
        else if (revents & EV_WRITE) {
            std::cout << "DoSend!" << "\n";
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
                        int nErr = errno;
                        if (nErr != EWOULDBLOCK && nErr != EMSGSIZE && nErr != EINTR && nErr != EINPROGRESS)
                        {
                            std::cout << "socket send error" << "\n";
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
private:
    int _listen_sockfd;
    std::string _shadow_ip;


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
    ServerNode(std::string ip_addr, int port) {
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
        _listen_watcher.set<ServerNode, &ServerNode::_listenSocketIOCallback> (this);
        _listen_watcher.start(_listen_sockfd, ev::READ);
    }
};


int main(int argc, char *argv[]) {

    std::cout << "Starting ISP-test-server" << "\n";

    puts_temp("test shadow_interface\n");

    ServerNode<Message, ReceiveMsg, ProcessMsg> serverNode("1.10.0.1", 18333);

    struct ev_loop *libev_loop = EV_DEFAULT;

    while (true) {
        std::cout << "before ev_run" << "\n";
        ev_run (libev_loop, EVRUN_ONCE);
    }
}
