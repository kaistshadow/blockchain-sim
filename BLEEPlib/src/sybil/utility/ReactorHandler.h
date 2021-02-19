//
// Created by ilios on 21. 2. 18..
//

#ifndef BLEEP_REACTORHANDLER_H
#define BLEEP_REACTORHANDLER_H

#include <deque>
#include <vector>
#include <assert.h>
#include "TCPControl.h"


namespace libBLEEP_sybil {

    template<class T>
    class BenignNode;

    template<class NodePrimitives>
    class BenignNodeDataSocketIOHandler {
    private:
        Reactor *_reactor; // singleton Reactor object
        BenignNode<NodePrimitives> *_node;

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
        void execute(ev::io &w, int revents) {
            if (revents & EV_READ) {

                // recv from socket, and append received data into the buffer.
                // typical socket buffer is 8K-64K
                char pchBuf[0x10000];
                int nBytes = 0;

                nBytes = recv(w.fd, pchBuf, sizeof(pchBuf), MSG_DONTWAIT); // What is the meaning of MSG_DONTWAIT?
                if (nBytes > 0) {
                    std::string recv_str(pchBuf, nBytes);

                    // call Node's primitive operation
                    _node->OpAfterRecv(w.fd, recv_str);
                } else if (nBytes == 0) {
                    std::cout << "connection closed while recv" << "\n";
                    close(w.fd);
                    _reactor->RemoveIOWatcher(&w);
                    _node->RemoveTCPControl(w.fd);
                    delete this;
                } else if (nBytes < 0) {
                    // error
                    std::cout << "Error while recv" << "\n";
                    exit(-1);
                }
            } else if (revents & EV_WRITE) {
                TCPControl &tcpBuffer = _node->GetTCPControl(w.fd);

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
                if (tcpBuffer.IsEmptySendBuffer()) {
                    assert(nSendOffset == 0);
                    // unset writable
                    w.set(w.fd, ev::READ);

                }
            }
        }

        BenignNodeDataSocketIOHandler(Reactor *reactor, BenignNode<NodePrimitives> *node) : _reactor(reactor),
                                                                                            _node(node) {}
    };

    template<class NodePrimitives>
    class BenignNodeConnSocketIOHandler {
    private:
        Reactor *_reactor; // singleton Reactor object
        BenignNode<NodePrimitives> *_node;
    public:
        void execute(ev::io &w, int revents) {
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
                _reactor->RemoveIOWatcher(&w);

                // Add watcher for data socket
                // assign an io event watcher for (connected) data socket descriptor
                ev::io *iowatcher = new ev::io;
                iowatcher->set<BenignNodeDataSocketIOHandler<NodePrimitives>, &BenignNodeDataSocketIOHandler<NodePrimitives>::execute>(
                        new BenignNodeDataSocketIOHandler(_reactor, _node));
                iowatcher->start(w.fd, ev::READ);
                _reactor->RegisterIOWatcher(iowatcher);

                // allocate new TCPControl structure
                _node->NewTCPControl(w.fd, iowatcher);

                // call Node's primitive operation
                _node->OpAfterConnect(w.fd);
            }
            delete this;
        }

        BenignNodeConnSocketIOHandler(Reactor *reactor, BenignNode<NodePrimitives> *node) : _reactor(reactor),
                                                                                            _node(node) {}
    };
}
#endif //BLEEP_REACTORHANDLER_H
