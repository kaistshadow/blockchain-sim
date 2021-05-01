#ifndef PROTOCOL_LAYER_POW_H
#define PROTOCOL_LAYER_POW_H

#include "../BL_MainEventManager.h"
#include "../BL2_peer_connectivity/PeerConnectivityLayer_API.h"
#include "ProtocolLayer_API.h"
#include "Transaction.h"
#include "TxGossipProtocol.h"
#include "POWBlock.h"
#include "POWMiner.h"
#include "BlockTree.h"
#include "BlockTree.cpp"  // This(BlockTree.cpp) is required since the BlockTree is template class

namespace libBLEEP_BL {

    // PoW parameters
//    extern int txNumPerBlock;
//    extern double txGenStartAt;
//    extern double txGenInterval;
//    extern double miningtime;
//    extern int miningnodecnt;

    class BL_ProtocolLayerPoW : public BL_ProtocolLayer_API {
    private:
        TxGossipProtocol _txGossipProtocol;
    private:
        POWMiner _powMiner;
        BlockTree<POWBlock> _blocktree;
    private: // PoW parameter
        int txNumPerBlock = 2;
        double txGenStartAt = 0;
        double txGenInterval = 4;
        double miningtime = 2;
        int miningnodecnt = 1;
    public:
        BlockTree<POWBlock>& GetBlockTree() { return _blocktree; }


        /* handler functions for each asynchronous event */
        void RecvMsgHandler(std::shared_ptr<Message> msg);

        void _RecvPOWBlockInvHandler(std::shared_ptr<Message> msg);

    private:
        std::shared_ptr<POWBlock> _makeCandidateBlock() {
            std::list<std::shared_ptr<SimpleTransaction> > txs = _txPool->GetTxs(txNumPerBlock);
            std::shared_ptr<POWBlock> candidateBlk = std::make_shared<POWBlock>("", txs);

            candidateBlk->SetBlockIdx(_blocktree.GetNextBlockIdx());
            candidateBlk->SetPrevBlockHash(_blocktree.GetLastHash());

            return candidateBlk;
        }

        // periodic tx generation for experimental purpose
        ev::timer _txgentimer;
        void _txgentimerCallback(ev::timer &w, int revents) {
            // generate random transaction
            srand((unsigned int)time(0));
            int sender_id = rand() % 100;
            int receiver_id = rand() % 100;
            float amount = (float) (rand() % 100000);
            std::shared_ptr<SimpleTransaction> tx = std::make_shared<SimpleTransaction>(sender_id, receiver_id, amount);

            if (!_txPool->ContainTx(tx->GetId())) {
                _txPool->AddTx(tx);
                _txGossipProtocol.PushTxToBroadcast(tx);

                if (_txPool->GetPendingTxNum() >= txNumPerBlock) {
                    if (!_powMiner.IsMining()) {
                        std::shared_ptr<POWBlock> candidateBlk = _makeCandidateBlock();
                        _powMiner.AsyncEmulateBlockMining(candidateBlk, miningtime/miningnodecnt);
                    }
                }
            }
            _txGenNum += 1;
        }
        void _startPeriodicTxGen(double start, double interval) {
            _txgentimer.set<BL_ProtocolLayerPoW, &BL_ProtocolLayerPoW::_txgentimerCallback>(this);
            _txgentimer.set(start, interval);
            _txgentimer.start();
        }


    public:
        BL_ProtocolLayerPoW();

        /* Switch asynchronous event to proper handler */
        virtual void SwitchAsyncEventHandler(AsyncEvent& event);

        /* public API functions */
        virtual bool InitiateProtocol();
        virtual bool InitiateProtocol(ProtocolParameter* params);
        virtual bool StopProtocol();
    };
}

#endif
