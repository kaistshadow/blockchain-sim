//
// Created by ilios on 21. 1. 13..
//

#include <iostream>


#include <ev++.h>
#include <memory>

#include <vector>
#include <list>
#include <deque>
#include <map>

#include <algorithm>
#include <arpa/inet.h>
#include <fcntl.h> /* Added for the nonblocking socket */
#include <assert.h>
#include <netinet/tcp.h>

#include "shadow_interface.h"

typedef unsigned int SOCKET;
#define INVALID_SOCKET      (SOCKET)(~0)
#define SOCKET_ERROR        -1

class TempMSG {
public:
    TempMSG() {}
};

template<typename MSG>
class SocketControlStruct {
private:
    SOCKET _socketfd;
    size_t _offset;
    bool fInbound;
    std::unique_ptr <ev::io> _connsocket_watcher;
    std::unique_ptr <ev::io> _datasocket_watcher;
    std::list <MSG> vRecvMsg;
    std::list <MSG> vProcessMsg;
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

template<typename MSG>
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

            std::cout << "server: got connection from " << inet_ntoa(their_addr.sin_addr) << ":"
                      << ntohs(their_addr.sin_port) << "\n";
/*
            // Create DataSocket and start a event watcher for the DataSocket
            mSocketControl.try_emplace(sock_fd, SocketControlStruct<MSG>(sock_fd, inet_ntoa(their_addr.sin_addr),
                                                                         ntohs(their_addr.sin_port)));
            std::cout << "server: got connection from " << mSocketControl[sock_fd].their_ip << ":"
                      << mSocketControl[sock_fd].their_port << "\n";

            ev::io &watcher = mSocketControl[sock_fd].getDataSocketWatcher();
            watcher.set<ActiveNode, &ActiveNode::_dataSocketIOCallback>(this);
            watcher.start(sock_fd, ev::READ);
*/
        } else {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                std::cout << "accept() failed errno=" << errno << strerror(errno) << "\n";
                exit(-1);
            }
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


//            ev::io &data_watcher = socketControl.getDataSocketWatcher();
//            data_watcher.set<ActiveNode, &ActiveNode::_dataSocketIOCallback>(this);
//            data_watcher.start(w.fd, ev::READ);

//            // send initializing message
//            std::deque<std::vector<unsigned char>> &vSendMsg = socketControl.getVSendMsg();
//            int ret = InitProtocol(vSendMsg, socketControl.their_ip, socketControl.their_port);
//            if (!ret) {
//                std::cout << "error while initializing protocol" << "\n";
//                exit(-1);
//            }
//            if (!vSendMsg.empty()) // set Writable
//                socketControl.getDataSocketWatcher().set(w.fd, ev::READ | ev::WRITE);
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

    ActiveNode(ActiveNode &&rhs) noexcept: _listen_sockfd(std::move(rhs._listen_sockfd)),
                                           _shadow_ip(std::move(rhs._shadow_ip)),
                                           mSocketControl(std::move(rhs.mSocketControl)) {
        rhs._listen_watcher.stop();
        _listen_watcher.set<ActiveNode, &ActiveNode::_listenSocketIOCallback>(this);
        _listen_watcher.start(_listen_sockfd, ev::READ);

        std::cout << "activeNode move constructor!" << "\n";
    }

    ActiveNode(const ActiveNode &rhs) = delete; // since ActiveNode includes watcher


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


        // Create socket and start a event watcher for the socket
        mSocketControl.try_emplace(remote_fd, SocketControlStruct<MSG>(remote_fd, inet_ntoa(servaddr.sin_addr),
                                                                       ntohs(servaddr.sin_port), false));
        std::cout << "node: try to connect to " << mSocketControl[remote_fd].their_ip << ":"
                  << mSocketControl[remote_fd].their_port << "\n";

        ev::io &watcher = mSocketControl[remote_fd].getConnSocketWatcher();
        watcher.set<ActiveNode, &ActiveNode::_connSocketIOCallback>(this);
        watcher.start(remote_fd, ev::WRITE);
    }

};

#define DEFAULT_SOCKET_PORT 3456

int main(int argc, char *argv[]) {
    std::cout << "sybil attacker launched" << "\n";

    std::vector <ActiveNode<TempMSG>> vActiveNode;
    vActiveNode.emplace_back("111.0.0.1", DEFAULT_SOCKET_PORT);
    vActiveNode.emplace_back("111.0.0.2", DEFAULT_SOCKET_PORT);
    vActiveNode.emplace_back("111.0.0.3", DEFAULT_SOCKET_PORT);
    vActiveNode.emplace_back("111.0.0.4", DEFAULT_SOCKET_PORT);
    vActiveNode.emplace_back("111.0.0.5", DEFAULT_SOCKET_PORT);

    for (auto &node : vActiveNode) {
        node.Connect("1.0.0.1", DEFAULT_SOCKET_PORT);
    }

    return 0;

}
