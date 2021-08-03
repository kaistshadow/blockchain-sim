// "Copyright [2021] <kaistshadow>"

#ifndef BLEEPEVAL_SYBILTEST_LIBRARY_SRC_POLICY_ECLIPSEINCOMINGATTACKPOLICY_H_
#define BLEEPEVAL_SYBILTEST_LIBRARY_SRC_POLICY_ECLIPSEINCOMINGATTACKPOLICY_H_

#include <list>

#include <utility>
#include <string>
#include <vector>
#include "../node/BenignNode.h"
#include "../node/ShadowActiveNode.h"
#include "utility/AttackStat.h"

using namespace std;

namespace sybiltest {
template<class NodePrimitives, class NodeParams>
class EclipseIncomingAttackPolicy {

public:
    EclipseIncomingAttackPolicy() : libev_loop(EV_DEFAULT) {
        _checkWatcher.set<EclipseIncomingAttackPolicy, &EclipseIncomingAttackPolicy<NodePrimitives, NodeParams>::_checkAttack>(
                this);
        _checkWatcher.start();
    }

    // step 1. construct virtual network using sybil nodes
    bool ConstructSybilNet(IPDatabase &ipdb, std::string targetIP, int targetPort) {
        vector<pair<string, int>> &vIPDurationPair = ipdb.GetIPDurationpair();
        vector<string> &vAttackerIP = ipdb.GetVAttackerIP();

        // Spawn network consisting of benign nodes
        for (auto &[ip, uptime] : vIPDurationPair) {
            auto &benignNode = _benignNodes.emplace_back(&_attackStat, &ipdb, ip);

            // set a churnout timer for all the benign nodes
            benignNode.SetChurnOutTimer(uptime);
        }
        std::cout << "benign node objects are constructed" << "\n";

        // Make benign nodes to connect to the victim
        for (auto &_benignNode : _benignNodes) {
            int conn_fd = _benignNode.tryConnectToTarget(targetIP, targetPort);
        }

        // Spawn network consisting of sybil nodes
        for (auto ip : vAttackerIP) {
            _attackerNodes.emplace_back(&_attackStat, &ipdb, ip);
            // The implementation of malicious node's periodic connection is dependent to node's behavior
            // Thus, periodic connection should be implemented within NodePrimitives
        }

        // Let sybil nodes try to connect to the victim 100 seconds later (i.e., after initializing benign networks)
        for (auto &_attackerNode : _attackerNodes) {
            _attackerNode.tryConnectToTarget(targetIP, targetPort, 100);
        }

        return true;
    }


 private:
    list <BenignNode<NodePrimitives>> _benignNodes;
    list <ShadowActiveNode<NodePrimitives>> _attackerNodes;
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
            ev_break(libev_loop, EVBREAK_ONE);
        }
    }

 protected:
    struct ev_loop *libev_loop = nullptr;
    ev::check _checkWatcher;
};
}  // namespace sybiltest

#endif  // BLEEPEVAL_SYBILTEST_LIBRARY_SRC_POLICY_ECLIPSEINCOMINGATTACKPOLICY_H_
