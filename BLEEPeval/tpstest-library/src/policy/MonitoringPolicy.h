#ifndef TPS_POLICY_H
#define TPS_POLICY_H

#include <list>
#include <ctime>
#include <chrono>
#include <iostream>

#include "../node/MonitoringNode.h"

using namespace std;

namespace tpstest {

    template<class NodePrimitives>
    class MonitoringPolicy {
    public:
        MonitoringPolicy() : libev_loop(EV_DEFAULT) {}

        // step 1. construct virtual network using sybil nodes
        bool ConstructNet (std::vector<pair<std::string,int>> targets, std::string NodeIP, int nodeport) {

            // Spawn network consisting of pre-defined number of transaction generator nodes
            auto &monitoringNode = _monitoringNodes.emplace_back(NodeIP, nodeport);

            std::cout << "monitoring nodes objects are constructed " << _monitoringNodes.size() << "\n";
            for (auto &_monitoringNode : _monitoringNodes) {
                for (auto &[_targetIP, _targetPort] : targets){
                    int conn_fd = _monitoringNode.tryConnectToTarget(_targetIP, _targetPort);
                }
                std::cout<<"constructNet!! is started and before start startRecevingNode\n";
                _monitoringNode.startReceivingNode(0);
            }

            return true;
        }

        list <MonitoringNode<NodePrimitives>> &GetMonitoringNodes() { return _monitoringNodes; }
    private:
        list <MonitoringNode<NodePrimitives>> _monitoringNodes;
    protected:
        struct ev_loop *libev_loop = nullptr;
    };
}

#endif
