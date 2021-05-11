#ifndef TPS_POLICY_H
#define TPS_POLICY_H

#include <list>
#include <ctime>
#include <chrono>
#include <iostream>

#include "../node/TxGeneratorNode.h"
#include "../node/MonitoringNode.h"
#include "../utility/blocks.h"

using namespace std;

namespace tpstest {

    template<class NodePrimitives>
    class TPSPolicy {
    public:
        TPSPolicy() : libev_loop(EV_DEFAULT) {}

        // step 1. construct virtual network using sybil nodes
        bool ConstructNet (std::vector<pair<std::string,int>> targets, std::string txgenIP, std::string monitorIP, int nodeport) {

            // Spawn network consisting of pre-defined number of transaction generator nodes
            auto &txGeneratorNode = _txGeneratorNodes.emplace_back(txgenIP, nodeport);

            std::cout << "generator node objects are constructed " << _txGeneratorNodes.size() << "\n";
            for (auto &_txGeneratorNode : _txGeneratorNodes) {
                _txGeneratorNode.bootstrap("./data/state.txt", "./data/key.txt");
                for (auto &[_targetIP, _targetPort] : targets){
                    int conn_fd = _txGeneratorNode.tryConnectToTarget(_targetIP, _targetPort);
                }
            }
            // spawn monitor node
            // Spawn network consisting of pre-defined number of transaction generator nodes
            auto &monitoringNode = _monitoringNodes.emplace_back(monitorIP, nodeport);
            for (auto &_monitoringNode : _monitoringNodes) {
                for (auto &[_targetIP, _targetPort] : targets){
                    int conn_fd = _monitoringNode.tryConnectToTarget(_targetIP, _targetPort);
                }
            }
            return true;
        }

        void SetGenerateTimer() {
#define TX_PER_TICK 1000        // temporary transaction per tick: 1000
#define TIME_PER_TICK   1       // temporary time per tick: 1 second
            for (auto &_txGeneratorNode : _txGeneratorNodes) {
                _txGeneratorNode.SetTxGenerateTimer(TX_PER_TICK, TIME_PER_TICK);
            }
        }

        void setMonitorTimer() {
            _monitorTimer.set<TPSPolicy,&TPSPolicy<NodePrimitives>::_txmonitorcb>(this);
            _monitorTimer.set(1,1);
            _monitorTimer.start();
        }

        void _txmonitorcb() {
            static block* best = nullptr;
            blockforest _bf;
            for (auto &_monitoringNode : _monitoringNodes) {
                _bf = _monitoringNode.get_blockforest();
            }
            block* bp = _bf.get_besttip();

            if (bp && bp->getParent() && (!best || best != bp)) {
                best = bp;
                uint32_t besttime = bp->getTime();
                size_t txcount = 1; //coinbase tx for genesis block
                int length_from_tip = 0;
                std::cout<<"blockhash="<<bp->getHexHash()<<" ";
                while(bp->getParent()) {
                    txcount += bp->getTxCount();
                    bp = bp->getParent();
                    length_from_tip++;
                }
                uint32_t timebase = bp->getTime();
                std::cout<<"\n";
                std::cout<<"besttime:"<<besttime<<"/";
                std::cout<<"timebase:"<<timebase<<"/"; 
                std::cout << "TPS="<< (txcount / ((double)besttime - timebase)) <<"/txcount="<<txcount<< "\n";
            }
        }


        list <TxGeneratorNode<NodePrimitives>> &GetTxGeneratorNodes() { return _txGeneratorNodes; }
        list <MonitoringNode<NodePrimitives>> &GetMonitoringNodes() { return _monitoringNodes; }
    private:
        list <TxGeneratorNode<NodePrimitives>> _txGeneratorNodes;
        list <MonitoringNode<NodePrimitives>> _monitoringNodes;

        ev::timer _monitorTimer;

    protected:
        struct ev_loop *libev_loop = nullptr;
    };
}

#endif
