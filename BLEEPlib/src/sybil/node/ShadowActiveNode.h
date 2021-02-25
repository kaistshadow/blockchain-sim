//
// Created by ilios on 21. 2. 19..
//

#ifndef BLEEP_SHADOWACTIVENODE_H
#define BLEEP_SHADOWACTIVENODE_H

#include "../utility/AttackStat.h"

namespace libBLEEP_sybil {
    template<class NodePrimitives>
    class ShadowActiveNode : public Node<NodePrimitives> {
    protected:
        void tryReconnectToTarget() override {
            assert (Node<NodePrimitives>::_targetIP != "" && Node<NodePrimitives>::_targetPort != -1);
            _connStartTimer.set<ShadowActiveNode<NodePrimitives>, &ShadowActiveNode<NodePrimitives>::_timercb>(this);
            _connStartTimer.set(30, 0);
            _connStartTimer.start();
        }

    private:
        ev::timer _connStartTimer;

        void _timercb(ev::timer &w, int revents) {
            // create new socket for a connection
            int conn_fd = CreateNewSocket();

            // bind to shadow's virtual NIC
            struct sockaddr_in new_addr;    /* my address information */
            new_addr.sin_family = AF_INET;         /* host byte order */
            new_addr.sin_port = htons(0);     /* short, network byte order, use random port */
            new_addr.sin_addr.s_addr = inet_addr(Node<NodePrimitives>::_myIP.c_str());
            bzero(&(new_addr.sin_zero), 8);        /* zero the rest of the struct */
            if (bind(conn_fd, (struct sockaddr *) &new_addr, sizeof(struct sockaddr)) == -1) {
                perror("bind");
                std::cout << "failed to bind" << "\n";
                exit(1);
            }

            struct sockaddr_in targetaddr;
            bzero(&targetaddr, sizeof(targetaddr));
            targetaddr.sin_family = AF_INET;
            targetaddr.sin_port = htons(Node<NodePrimitives>::_targetPort);
            targetaddr.sin_addr.s_addr = inet_addr(Node<NodePrimitives>::_targetIP.c_str());

            // call actual systemcall API for connect
            // TODO : error-handling logic should be updated
            int ret = connect(conn_fd, (struct sockaddr *) &targetaddr, sizeof(targetaddr));
            if (ret < 0 && errno != EINPROGRESS) {
                perror("connect");
                std::cout << "Unable to connect to " << Node<NodePrimitives>::_targetIP << "\n";
                exit(-1);
            } else if (ret == 0) {
                std::cout << "connection established" << "\n";
                std::cout << "non-blocking socket is immediately established. is it possible?" << "\n";
                exit(-1);
            } else if (ret > 0) {
                std::cout << "unexpected return value of non-blocking connect" << "\n";
                exit(-1);
            }

            // assign an io event watcher for (connect) tried socket descriptor
            // and register an event watcher to monitor for the beginning of I/O operation (A.K.A reactor pattern)
            auto[it, result] = Node<NodePrimitives>::_mConnSocketWatcher.try_emplace(conn_fd);
            if (result) {
                ev::io &watcher = it->second;
                watcher.set<Node<NodePrimitives>, &Node<NodePrimitives>::conncb>(this);
                watcher.set(conn_fd, ev::WRITE);
                watcher.start();
            }

            _connStartTimer.stop();
        }

    public:
        ShadowActiveNode(AttackStat *stat, IPDatabase *ipdb, std::string virtualIp) : Node<NodePrimitives>(stat, ipdb,
                                                                                                           virtualIp, 0,
                                                                                                           NodeType::Attacker) {}

        // move constructor
        ShadowActiveNode(ShadowActiveNode &&other) = default;

        // API for connection to target
        void tryConnectToTarget(std::string targetIP, int targetPort, double starttime) {
            _connStartTimer.set<ShadowActiveNode<NodePrimitives>, &ShadowActiveNode<NodePrimitives>::_timercb>(this);
            _connStartTimer.set(starttime, 0);
            _connStartTimer.start();

            Node<NodePrimitives>::_targetIP = targetIP;
            Node<NodePrimitives>::_targetPort = targetPort;
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

#endif //BLEEP_SHADOWACTIVENODE_H
