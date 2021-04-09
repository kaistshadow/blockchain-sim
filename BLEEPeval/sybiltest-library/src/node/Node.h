//
// Created by ilios on 21. 2. 17..
//

#ifndef BLEEP_NODE_H
#define BLEEP_NODE_H

#include <iostream>
#include <map>
#include <assert.h>
#include "../utility/TCPControl.h"
#include <ev++.h>

#include <arpa/inet.h>
#include <fcntl.h> /* Added for the nonblocking socket */
#include <unistd.h>
#include <netinet/tcp.h>

#include "../utility/AttackStat.h"
#include "shadow_interface.h"
#include "../ipdb/IPDatabase.h"

#include <ctime>
#include <chrono>


namespace sybiltest {
    enum class NodeType {
        Benign, Attacker, Shadow
    };

    template<class NodePrimitives>
    class Node : public NodePrimitives {
    protected:
        std::map<int, ev::io> _mDataSocketWatcher;
        std::map<int, ev::io> _mConnSocketWatcher;
        ev::io _listen_watcher;
        int _listen_sockfd;

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

    public:
        void readcb(ev::io &w) {
            // recv from socket, and append received data into the buffer.
            // typical socket buffer is 8K-64K
            char pchBuf[0x10000];
            int nBytes = 0;

            nBytes = recv(w.fd, pchBuf, sizeof(pchBuf), MSG_DONTWAIT); // What is the meaning of MSG_DONTWAIT?
            if (nBytes > 0) {
                std::string recv_str(pchBuf, nBytes);

                // call Node's primitive operation
                NodePrimitives::OpAfterRecv(w.fd, recv_str);
            } else if (nBytes == 0) {
                std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                std::string s(30, '\0');
                std::strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
                std::cout << "connection closed time:" << s << "\n";
                std::cout << "connection closed while recv" << ", myIP=" << NodePrimitives::GetIP() << "\n";

                // remove data structure for this socket
                close(w.fd);
                NodePrimitives::RemoveTCPControl(w.fd);
                _mDataSocketWatcher.erase(w.fd);


                // call Node's primitive operation
                NodePrimitives::OpAfterDisconnect();

            } else if (nBytes < 0) {
                // error
                std::cout << "Error while recv" << "\n";
                exit(-1);
            }
        }

        void writecb(ev::io &w) {
            TCPControl &tcpBuffer = NodePrimitives::GetTCPControl(w.fd);

            size_t nSentSize = 0;
            size_t nSendOffset = tcpBuffer.getSendOffset();
            while (!tcpBuffer.IsEmptySendBuffer()) {
                const std::vector<unsigned char> &buff = tcpBuffer.FrontSendBuffer();
                assert(buff.size() > nSendOffset);

                int nBytes = send(w.fd, reinterpret_cast<const char *>(buff.data()) + nSendOffset,
                                  buff.size() - nSendOffset, MSG_NOSIGNAL | MSG_DONTWAIT);
                if (nBytes > 0) {
                    nSendOffset += nBytes;
                    nSentSize += nBytes;
                    if (nSendOffset == buff.size()) {
                        nSendOffset = 0;
                        tcpBuffer.RemoveFrontSendBuffer();
                    } else {
                        // could not send full message; stop sending more
                        break;
                    }
                } else {
                    if (nBytes < 0) {
                        // error
                        int nErr = errno;
                        if (nErr != EWOULDBLOCK && nErr != EMSGSIZE && nErr != EINTR && nErr != EINPROGRESS) {
                            std::cout << "socket send error" << "\n";
                            exit(-1);
                        }
                    }
                    // couldn't send anything at all
                    break;
                }
            }
            tcpBuffer.setSendOffset(nSendOffset);
        }

        void datacb(ev::io &w, int revents) {
            if (revents & EV_READ) {
                readcb(w);
            } else if (revents & EV_WRITE) {
                writecb(w);
            }

            UpdateDataSocketWatcher();
        }

        void conncb(ev::io &w, int revents) {
            std::cout << "connSocketIOhandler" << "\n";
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
                    return;
                }

                std::cout << "Socket is successfully connected" << "\n";

                // Add watcher for data socket
                // assign an io event watcher for (connected) data socket descriptor
                auto[it, result] = _mDataSocketWatcher.try_emplace(w.fd);
                if (result) {
                    ev::io &watcher = it->second;
                    watcher.set<Node, &Node::datacb>(this);
                    watcher.set(w.fd, ev::READ);
                    watcher.start();
                }
                // allocate new TCPControl structure
                NodePrimitives::NewTCPControl(w.fd);

                // call Node's primitive operation
                NodePrimitives::OpAfterConnect(w.fd);

                // remove watcher for this conn socket
                _mConnSocketWatcher.erase(w.fd);
            }

            UpdateDataSocketWatcher();
        }

        void listencb(ev::io &w, int revents) {
            struct sockaddr_in their_addr; /* connector's address information */
            int sock_fd;
            socklen_t sin_size;

            sin_size = sizeof(struct sockaddr_in);
            sock_fd = accept(w.fd, (struct sockaddr *) &their_addr, &sin_size);
            if (sock_fd != -1) {
                fcntl(sock_fd, F_SETFL, O_NONBLOCK);

                // Add watcher for data socket
                // assign an io event watcher for (connected) data socket descriptor
                auto[it, result] = _mDataSocketWatcher.try_emplace(sock_fd);
                if (result) {
                    ev::io &watcher = it->second;
                    watcher.set<Node, &Node::datacb>(this);
                    watcher.set(sock_fd, ev::READ);
                    watcher.start();
                }
                // allocate new TCPControl structure
                NodePrimitives::NewTCPControl(sock_fd);

                // call Node's primitive operation
                NodePrimitives::OpAfterConnected(sock_fd);
            } else {
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    std::cout << "accept() failed errno=" << errno << strerror(errno) << "\n";
                    exit(-1);
                }
            }

            UpdateDataSocketWatcher();
        }

        void UpdateDataSocketWatcher() {
            for (auto const &[fd, tcpControl] : NodePrimitives::_mTCPControl) {
                if (tcpControl.IsEmptySendBuffer()) {
                    _mDataSocketWatcher[fd].set(ev::READ);
                } else {
                    _mDataSocketWatcher[fd].set(ev::READ | ev::WRITE);
                }
            }
        }

    public:
        Node(AttackStat *stat, IPDatabase *ipdb, std::string myIP, int listenPort, NodeType type) : NodePrimitives(stat,
                                                                                                                   ipdb,
                                                                                                                   myIP,
                                                                                                                   type) {
            // Create virtual NIC for this node
            struct sockaddr_in my_addr;    /* my address information */
            my_addr.sin_family = AF_INET;         /* host byte order */
            my_addr.sin_addr.s_addr = inet_addr(myIP.c_str());
            bzero(&(my_addr.sin_zero), 8);        /* zero the rest of the struct */
            if (shadow_register_NIC((struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) {
                std::cout << "shadow_register_NIC failed" << "\n";
                exit(-1);
            }

            // create listen socket
            if (listenPort == 0)
                return;
            int listenfd = CreateNewSocket();

            // bind to shadow's virtual NIC
            my_addr.sin_family = AF_INET;         /* host byte order */
            my_addr.sin_port = htons(listenPort);     /* short, network byte order */
            my_addr.sin_addr.s_addr = inet_addr(myIP.c_str());
            bzero(&(my_addr.sin_zero), 8);        /* zero the rest of the struct */

            if (bind(listenfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) {
                perror("bind");
                exit(1);
            }

            // listen
            if (listen(listenfd, 10) == -1) {
                perror("listen");
                exit(1);
            }

            // register a watcher
            _listen_sockfd = listenfd;
            _listen_watcher.set<Node, &Node<NodePrimitives>::listencb>(this);
            _listen_watcher.start(_listen_sockfd, ev::READ);
        }

    private:

        int CreateNewSocket() {
            // Create a TCP socket in the address family of the specified service.
            unsigned int hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (hSocket == -1)
                return -1;

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

        bool SetSocketNonBlocking(const unsigned int &hSocket, bool fNonBlocking) {
            if (fNonBlocking) {
                int fFlags = fcntl(hSocket, F_GETFL, 0);
                if (fcntl(hSocket, F_SETFL, fFlags | O_NONBLOCK) == -1) {
                    return false;
                }
            } else {
                int fFlags = fcntl(hSocket, F_GETFL, 0);
                if (fcntl(hSocket, F_SETFL, fFlags & ~O_NONBLOCK) == -1) {
                    return false;
                }
            }

            return true;
        }

        bool CloseSocket(unsigned int &hSocket) {
            if (hSocket == -1)
                return false;
            int ret = close(hSocket);
            if (ret) {
                std::cout << "Socket close failed: " << hSocket << "\n";
                exit(-1);
            }
            hSocket = -1;
            return ret != -1;
        }
    };
}

#endif //BLEEP_NODE_H
