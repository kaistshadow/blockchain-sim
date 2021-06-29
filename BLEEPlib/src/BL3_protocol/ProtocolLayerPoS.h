//
// Created by Hyunjin Kim on 2021/06/04.
//

#ifndef PROTOCOL_LAYER_POS_H
#define PROTOCOL_LAYER_POS_H

#include "../BL_MainEventManager.h"
#include "../BL2_peer_connectivity/PeerConnectivityLayer_API.h"
#include "ProtocolLayer_API.h"
#include "Transaction.h"
#include "TxGossipProtocol.h"
#include "POSBlock.h"
#include "POSMiner.h"
#include "BlockTree.h"
#include "BlockTree.cpp"  // This(BlockTree.cpp) is required since the BlockTree is template class
#include "POSStakes.h"

#include <string>

namespace libBLEEP_BL {
    class BL_ProtocolLayerPoS : public BL_ProtocolLayer_API {
    private:
        TxGossipProtocol _txGossipProtocol;
        StakeList stakes;
        unsigned long _creatorNodeId;

        POSMiner _posMiner;
        BlockTree<POSBlock> _blocktree;

    public:
        BlockTree<POSBlock>& GetBlockTree() { return _blocktree; }
    private: // PoS parameter
        unsigned int maxTxPerBlock = 100;
        double txGenStartAt = 0;
        double txGenInterval = 4;

        double slot_interval = 20;
        std::string stakeDatafile = "stakes.txt";
        unsigned int slot_epoch_cnt = 10;
        unsigned int chain_selection_block_threshold = 4;    // chain selection: 
    private: // Block propagation protocol-related data structure (processing inventory)
        std::vector <std::string> _processingBlkinv;
        bool _processing = false;
        bool HasProcessingInv() { return _processing; }
        void StartProcessingInv() { _processing = true; }
        void StopProcessingInv() { _processing = false; }
        std::vector <std::string>& GetProcessingInv() { return _processingBlkinv; }
        void SetProcessingInv(std::vector <std::string> inv) { _processingBlkinv = inv; }


        /* handler functions for each asynchronous event */
        void RecvMsgHandler(std::shared_ptr<Message> msg);

        void _RecvPOSBlockInvHandler(std::shared_ptr<Message> msg);
        void _RecvPOSBlockGetBlocksHandler(std::shared_ptr<Message> msg);
        void _RecvPOSBlockGetDataHandler(std::shared_ptr<Message> msg);
        void _RecvPOSBlockBlkHandler(std::shared_ptr<Message> msg);

    private:
        std::shared_ptr<POSBlock> makeBlockTemplate(unsigned long slot_id);

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
            }
            _txGenNum += 1;
        }
        void _startPeriodicTxGen(double start, double interval) {
            _txgentimer.set<BL_ProtocolLayerPoS, &BL_ProtocolLayerPoS::_txgentimerCallback>(this);
            _txgentimer.set(start, interval);
            _txgentimer.start();
        }

        // slot processing
        ev::timer _slottimer;
        void _slottimerCallback(ev::timer &w, int revents);
        void _startPeriodicSlotStart(double interval);


    public:
        BL_ProtocolLayerPoS();

        /* Switch asynchronous event to proper handler */
        virtual void SwitchAsyncEventHandler(AsyncEvent& event);

        /* public API functions */
        virtual bool InitiateProtocol();
        virtual bool InitiateProtocol(ProtocolParameter* params);
        virtual bool StopProtocol();

        unsigned long random_selection(unsigned long slot_id);
    };
}

#endif
