#ifndef ECLIPSE_INCOMING_ATTACK_POLICY_H
#define ECLIPSE_INCOMING_ATTACK_POLICY_H

#include <list>

#include "utility/Reactor.h"
#include "../node/BenignNode.h"

using namespace std;

namespace libBLEEP_sybil {

    template<class NodePrimitives>
    class EclipseIncomingAttackPolicy {

    public:
        // step 1. construct virtual network using sybil nodes
        bool ConstructSybilNet(vector<pair<string, int>> vIP, std::string targetIP, int targetPort) {
            // Spawn network consisting of benign nodes
            for (auto &[ip, uptime] : vIP) {
                _benignNodes.emplace_back(ip);
                //TODO : should implement churnout
            }
            std::cout << "benign node objects are constructed" << "\n";

            // Make benign nodes to connect to the victim
            for (auto &_benignNode : _benignNodes) {
                int conn_fd = _benignNode.tryConnectToTarget(targetIP, targetPort);

                // assign an io event watcher for (connect) tried socket descriptor
                // and register an event watcher to monitor for the beginning of I/O operation (A.K.A reactor pattern)
                ev::io *iowatcher = new ev::io;
                iowatcher->set<BenignNodeConnSocketIOHandler<NodePrimitives>, &BenignNodeConnSocketIOHandler<NodePrimitives>::execute>(
                        new BenignNodeConnSocketIOHandler(&_reactor, &_benignNode));
                iowatcher->start(conn_fd, ev::WRITE);
                _reactor.RegisterIOWatcher(iowatcher);
            }

            // Spawn network consisting of sybil nodes

            return true;
        }

        // step 2. Initiate malicious node's periodic connection


        // step 3. Run event loop
        void RunEventLoop() {
            _reactor.HandleEvents();
        }

    private:
        list <BenignNode<NodePrimitives>> _benignNodes;
        Reactor _reactor;
    };
}

#endif
