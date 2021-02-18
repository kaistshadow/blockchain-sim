//
// Created by ilios on 21. 2. 16..
//

#ifndef BLEEP_REACTOR_H
#define BLEEP_REACTOR_H

#include <ev++.h>
#include <memory>
#include <vector>
#include <node/BenignNode.h>


namespace libBLEEP_sybil {
    class Reactor {
    public:
        Reactor() { libev_loop = EV_DEFAULT; }

        void HandleEvents();

        bool RegisterIOWatcher(ev::io *iowatcher);

        bool RemoveIOWatcher(ev::io *iowatcher);

        bool RegisterTimerWatcher(ev::timer *timerwatcher);

        bool RemoveTimerWatcher(ev::timer *timerwatcher);

    private:
        std::vector<ev::io *> _vIOWatcher;
        std::vector<ev::timer *> _vTimerWatcher;
        static struct ev_loop *libev_loop;
    };


    template<class NodePrimitives>
    class BenignNodeConnSocketIOHandler {
    private:
        Reactor *_reactor;
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
                _node->OpAfterConnect();
            }
            delete this;
        }

        BenignNodeConnSocketIOHandler(Reactor *reactor, BenignNode<NodePrimitives> *node) : _reactor(reactor),
                                                                                            _node(node) {}
    };
}

#endif //BLEEP_REACTOR_H
