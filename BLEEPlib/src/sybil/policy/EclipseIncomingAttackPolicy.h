#ifndef ECLIPSE_INCOMING_ATTACK_POLICY_H
#define ECLIPSE_INCOMING_ATTACK_POLICY_H

#include <list>

#include "../node/BenignNode.h"
#include "../node/ShadowActiveNode.h"
#include "utility/AttackStat.h"

using namespace std;

namespace libBLEEP_sybil {

    template<class NodePrimitives>
    class EclipseIncomingAttackPolicy {

    public:
        EclipseIncomingAttackPolicy() {}

        // step 1. construct virtual network using sybil nodes
        bool ConstructSybilNet(vector<pair<string, int>> vIP, vector<string> vShadowIP, std::string targetIP,
                               int targetPort) {
            // Spawn network consisting of benign nodes
            for (auto &[ip, uptime] : vIP) {
                _benignNodes.emplace_back(ip);
                //TODO : should implement churnout
            }
            std::cout << "benign node objects are constructed" << "\n";

            // Make benign nodes to connect to the victim
            for (auto &_benignNode : _benignNodes) {
                int conn_fd = _benignNode.tryConnectToTarget(targetIP, targetPort);
            }

            // Spawn network consisting of sybil nodes
            for (auto ip : vShadowIP) {
                _shadowNodes.emplace_back(ip);
            }

            // Let sybil nodes try to connect to the victim 100 seconds later (i.e., after initializing benign networks)
            for (auto &_shadowNode : _shadowNodes) {
                _shadowNode.tryConnectToTarget(targetIP, targetPort, 100);
            }

            return true;
        }

        // step 2. Initiate malicious node's periodic connection



    private:
        list <BenignNode<NodePrimitives>> _benignNodes;
        list <ShadowActiveNode<NodePrimitives>> _shadowNodes;
    };
}

#endif
