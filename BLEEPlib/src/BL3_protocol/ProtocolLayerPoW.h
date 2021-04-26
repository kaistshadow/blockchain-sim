#ifndef PROTOCOL_LAYER_POW_H
#define PROTOCOL_LAYER_POW_H

#include "../BL_MainEventManager.h"
#include "../BL2_peer_connectivity/PeerConnectivityLayer_API.h"
#include "ProtocolLayer_API.h"
#include "Transaction.h"
#include "TxGossipProtocol.h"
#include "POWBlock.h"
#include "POWMiner.h"


namespace libBLEEP_BL {

    // PoW parameters
    extern int txNumPerBlock;
    extern double txGenStartAt;
    extern double txGenInterval;
    extern double miningtime;
    extern int miningnodecnt;

    class BL_ProtocolLayerPoW : public BL_ProtocolLayer_API {
    private:
        POWMiner _powMiner;

        /* handler functions for each asynchronous event */
        void RecvMsgHandler(std::shared_ptr<Message> msg);

    private:
        std::shared_ptr<POWBlock> _makeCandidateBlock() {
            std::list<std::shared_ptr<SimpleTransaction> > txs = _txPool->GetTxs(txNumPerBlock);
            std::shared_ptr<POWBlock> candidateBlk = std::make_shared<POWBlock>("", txs);

            unsigned long blockidx = _blkPool.size();
            candidateBlk->SetBlockIdx(blockidx);
            std::shared_ptr<POWBlock> lastblk = std::dynamic_pointer_cast<POWBlock>(GetLastBlock());
            if (lastblk)
                candidateBlk->SetPrevBlockHash(lastblk->GetBlockHash());

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
            SimpleTransaction tx(sender_id,receiver_id,amount);

            if (!_txPool->ContainTx(tx.GetId())) {
                _txPool->AddTx(std::make_shared<SimpleTransaction>(tx));

                if (_txPool->GetPendingTxNum() >= libBLEEP_BL::txNumPerBlock) {
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
        virtual bool StopProtocol();
    };
}

#endif
