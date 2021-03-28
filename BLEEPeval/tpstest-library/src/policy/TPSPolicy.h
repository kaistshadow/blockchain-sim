#ifndef TPS_POLICY_H
#define TPS_POLICY_H

#include <list>
#include <ctime>
#include <chrono>
#include <iostream>

#include "../node/TxGeneratorNode.h"

using namespace std;

namespace tpstest {

    template<class NodePrimitives>
    class TPSPolicy {
    public:
        TPSPolicy() : libev_loop(EV_DEFAULT) {}

        // step 1. construct virtual network using sybil nodes
        bool ConstructNet (std::vector<pair<std::string,int>> targets, std::string NodeIP, int nodeport) {

            // Spawn network consisting of pre-defined number of transaction generator nodes
            auto &txGeneratorNode = _txGeneratorNodes.emplace_back(NodeIP, nodeport);

#define TX_PER_TICK 1000        // temporary transaction per tick: 1000
#define TIME_PER_TICK   1       // temporary time per tick: 1 second
            std::cout << "generator node objects are constructed " << _txGeneratorNodes.size() << "\n";
            for (auto &_txGeneratorNode : _txGeneratorNodes) {
                _txGeneratorNode.bootstrap("state.txt", "key.txt");
                for (auto &[_targetIP, _targetPort] : targets){
                    int conn_fd = _txGeneratorNode.tryConnectToTarget(_targetIP, _targetPort);
                }
                _txGeneratorNode.SetTxGenerateTimer(TX_PER_TICK, TIME_PER_TICK);
            }

            return true;
        }

        list <TxGeneratorNode<NodePrimitives>> &GetTxGeneratorNodes() { return _txGeneratorNodes; }
    private:
        list <TxGeneratorNode<NodePrimitives>> _txGeneratorNodes;
    protected:
        struct ev_loop *libev_loop = nullptr;
    };
}

#endif