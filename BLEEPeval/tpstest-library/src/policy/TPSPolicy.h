#ifndef TPS_POLICY_H
#define TPS_POLICY_H

#include <list>
#include <ctime>
#include <chrono>
#include <iostream>

#include "../node/BenignNode.h"

using namespace std;

namespace tpstest {

    template<class NodePrimitives>
    class TPSPolicy {

    public:
      TPSPolicy() : libev_loop(EV_DEFAULT) {
            _checkWatcher.set<TPSPolicy, &TPSPolicy<NodePrimitives>::_checkAttack>(this);
            _checkWatcher.start();
        }

        // step 1. construct virtual network using sybil nodes
        bool ConstructNet (std::vector<pair<std::string,int>> targets, std::string NodeIP, int nodeport) {

            // Spawn network consisting of pre-defined number of benign nodes
            auto &benignNode = _benignNodes.emplace_back(NodeIP, nodeport);

            std::cout << "benign node objects are constructed " << _benignNodes.size() << "\n";
          for (auto &_benignNode : _benignNodes) {
            for (auto &[_targetIP, _targetPort] : targets){
                int conn_fd = _benignNode.tryConnectToTarget(_targetIP, _targetPort);
            }
          }


          // Let attacker node to periodically call ADDR injection API
            _addrInjectTimer.set<TPSPolicy, &TPSPolicy::_timercb>(this);
            _addrInjectTimer.start();

            return true;
        }

        list <BenignNode<NodePrimitives>> &GetBenignNodes() { return _benignNodes; }



    private:
        void _timercb(ev::timer &w, int revents) {

        }

        ev::timer _addrInjectTimer;

    private:
        list <BenignNode<NodePrimitives>> _benignNodes;


    private:
      void _checkAttack() {
        // If we hijacked victim's incoming connection more than predefined target number,
        // it means that attack is successful, so break the main loop.
//        if (_attackStat.GetHijackedOutgoingConnNum() >= NodeParams::targetOutgoingConnNum) {
//          std::cout << "The EREBUS attack is succeeded." << "\n";
//          std::cout << "Pre-defined target outgoing connection num = " << NodeParams::targetOutgoingConnNum
//                    << "\n";
//          std::cout << "Hijacked outgoing connection num = " << _attackStat.GetHijackedOutgoingConnNum() << "\n";
//
//          std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
//          std::string s(30, '\0');
//          std::strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
//          std::cout << "End time:" << s << "\n";
//
//          ev_break(libev_loop, EVBREAK_ONE);
//        }
      }


     protected:
        struct ev_loop *libev_loop = nullptr;
        ev::check _checkWatcher;
    };
}

#endif
