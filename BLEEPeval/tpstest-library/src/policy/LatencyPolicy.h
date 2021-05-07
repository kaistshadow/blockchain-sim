#ifndef LATENCY_POLICY_H
#define LATENCY_POLICY_H

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
    class LatencyPolicy {
    public:
        LatencyPolicy() : libev_loop(EV_DEFAULT) {}

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
#define TX_PER_TICK 10        // temporary transaction per tick: 1000
#define TIME_PER_TICK   1       // temporary time per tick: 1 second
            for (auto &_txGeneratorNode : _txGeneratorNodes) {
                _txGeneratorNode.SetTxGenerateTimer(TX_PER_TICK, TIME_PER_TICK);
            }
        }

        void setMonitorTimer() {
            _monitorTimer.set<LatencyPolicy,&LatencyPolicy<NodePrimitives>::txmonitorcb>(this);
            _monitorTimer.set(1,1);
            _monitorTimer.start();
        }

        void txmonitorcb() {
            blockforest _bf;
            for (auto &_monitoringNode : _monitoringNodes) {
                _bf = _monitoringNode.get_blockforest();
            }
            block* bp = _bf.get_besttip();
            if(!bp) {
                return;
            }

            unsigned long int netTxLatency = 0;
            std::vector<std::string> txs = bp->getTxHashes();

            uint32_t time = bp->getTime();

            if (txs.size() >= 2) {
                for (int i = 1; i<txs.size(); i++) {
                    assert(time >= global_txtimepool->get_txtime(txs[i]));
                    netTxLatency += time - global_txtimepool->get_txtime(txs[i]);
                }
                bp->setNetTxLatency(netTxLatency);
            }

            static block* best = nullptr;


#define CONFIRMATION_COUNT  4
            if (bp && bp->getParent() && (!best || best != bp)) {
                best = bp;
                int length_from_tip = 0;
                size_t netConfirmedTxCount = 0;
                unsigned long int netConfirmedTxLatency = 0;

                while(bp->getParent()) {
                    if (length_from_tip > CONFIRMATION_COUNT) {
                        netConfirmedTxCount += bp->getTxCount();    // ignore coinbase tx
                        netConfirmedTxLatency += bp->getNetTxLatency();
                    }
                    bp = bp->getParent();
                    length_from_tip++;
                }
                if (netConfirmedTxCount && netConfirmedTxLatency) {
                    std::cout << "Latency = " << (netConfirmedTxLatency / netConfirmedTxCount) << "seconds\n";

                }
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
