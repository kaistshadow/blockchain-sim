//
// Created by ilios on 21. 2. 19..
//

#ifndef BLEEP_SHADOWACTIVENODE_H
#define BLEEP_SHADOWACTIVENODE_H

namespace libBLEEP_sybil {
    template<class NodePrimitives>
    class ShadowActiveNode : public Node<NodePrimitives> {
    protected:
        void tryReconnectToTarget() override {
            assert (_targetIP != "" && _targetPort != -1);
            _connStartTimer.set<ShadowActiveNode<NodePrimitives>, &ShadowActiveNode<NodePrimitives>::_timercb>(this);
            _connStartTimer.set(30, 0);
            _connStartTimer.start();
        }

    private:
        ev::timer _connStartTimer;
        std::string _targetIP = "";
        int _targetPort = -1;

        void _timercb(ev::timer &w, int revents) {
            // create new socket for a connection
            int conn_fd = CreateNewSocket();

            struct sockaddr_in targetaddr;
            bzero(&targetaddr, sizeof(targetaddr));
            targetaddr.sin_family = AF_INET;
            targetaddr.sin_port = htons(_targetPort);
            targetaddr.sin_addr.s_addr = inet_addr(_targetIP.c_str());

            // call actual systemcall API for connect
            // TODO : error-handling logic should be updated
            int ret = connect(conn_fd, (struct sockaddr *) &targetaddr, sizeof(targetaddr));
            if (ret < 0 && errno != EINPROGRESS) {
                perror("connect");
                std::cout << "Unable to connect to " << _targetIP << "\n";
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
        ShadowActiveNode(std::string virtualIp) : Node<NodePrimitives>(virtualIp, NodeType::Shadow) {
            // Create virtual NIC for this node
            struct sockaddr_in my_addr;    /* my address information */
            my_addr.sin_family = AF_INET;         /* host byte order */
            my_addr.sin_addr.s_addr = inet_addr(Node<NodePrimitives>::_myIP.c_str());
            bzero(&(my_addr.sin_zero), 8);        /* zero the rest of the struct */
            if (shadow_register_NIC((struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) {
                std::cout << "shadow_register_NIC failed" << "\n";
                exit(-1);
            }
        }

        // move constructor
        ShadowActiveNode(ShadowActiveNode &&other) = default;

        // API for connection to target
        void tryConnectToTarget(std::string targetIP, int targetPort, double starttime) {
            _connStartTimer.set<ShadowActiveNode<NodePrimitives>, &ShadowActiveNode<NodePrimitives>::_timercb>(this);
            _connStartTimer.set(starttime, 0);
            _connStartTimer.start();

            _targetIP = targetIP;
            _targetPort = targetPort;
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
