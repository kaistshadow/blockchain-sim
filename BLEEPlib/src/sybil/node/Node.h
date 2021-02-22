//
// Created by ilios on 21. 2. 17..
//

#ifndef BLEEP_NODE_H
#define BLEEP_NODE_H

#include <map>
#include <assert.h>
#include "../utility/TCPControl.h"
#include <ev++.h>

#include <arpa/inet.h>
#include <fcntl.h> /* Added for the nonblocking socket */
#include <unistd.h>
#include <netinet/tcp.h>

#include "../utility/AttackStat.h"


namespace libBLEEP_sybil {
    enum class NodeType {
        Benign, Shadow
    };

    template<class NodePrimitives>
    class Node : public NodePrimitives {
    protected:
        std::map<int, ev::io> _mDataSocketWatcher;
        std::map<int, ev::io> _mConnSocketWatcher;

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
                std::cout << "connection closed while recv" << ", myIP=" << NodePrimitives::GetIP() << "\n";

                // remove data structure for this socket
                close(w.fd);
                NodePrimitives::RemoveTCPControl(w.fd);
                _mDataSocketWatcher.erase(w.fd);

                if (NodePrimitives::_type == NodeType::Shadow) {
                    // call Node's primitive operation
                    NodePrimitives::OpAfterDisconnect();
                }
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

            for (auto const &[fd, tcpControl] : NodePrimitives::_mTCPControl) {
                if (tcpControl.IsEmptySendBuffer()) {
                    _mDataSocketWatcher[fd].set(ev::READ);
                } else {
                    _mDataSocketWatcher[fd].set(ev::READ | ev::WRITE);
                }
            }
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

            for (auto const &[fd, tcpControl] : NodePrimitives::_mTCPControl) {
                if (tcpControl.IsEmptySendBuffer()) {
                    _mDataSocketWatcher[fd].set(ev::READ);
                } else {
                    _mDataSocketWatcher[fd].set(ev::READ | ev::WRITE);
                }
            }
        }

    public:
        Node(AttackStat *stat, std::string vIP, NodeType type) : NodePrimitives(stat, vIP, type) {}

    };
}

#endif //BLEEP_NODE_H
