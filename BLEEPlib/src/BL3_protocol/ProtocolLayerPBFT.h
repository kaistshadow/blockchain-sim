//
// Created by Hyunjin Kim on 2021/06/09.
//

#ifndef PROTOCOL_LAYER_PBFT_H
#define PROTOCOL_LAYER_PBFT_H

#include "../BL_MainEventManager.h"
#include "../BL2_peer_connectivity/PeerConnectivityLayer_API.h"
#include "ProtocolLayer_API.h"
#include "Transaction.h"
#include "TxGossipProtocol.h"
#include "PBFTBlock.h"
#include "PBFTMiner.h"
#include "BlockTree.h"
#include "BlockTree.cpp"  // This(BlockTree.cpp) is required since the BlockTree is template class
#include "PBFTConfig.h"

#include "shadow_memshare_interface.h"

namespace libBLEEP_BL {
    class BL_ProtocolLayerPBFT : public BL_ProtocolLayer_API {
    private:
        TxGossipProtocol _txGossipProtocol;
    private:
        BlockTree<PBFTBlock> _blocktree;
    public:
        BlockTree<PBFTBlock>& GetBlockTree() { return _blocktree; }
    private:
        // PBFT parameter
        int txNumPerBlock = 2;
        double txGenStartAt = 0;
        double txGenInterval = 4;
        unsigned long consensusId = 0;
        std::string configFile = "config.txt";
        double phase_interval = 20;

    private:
        // Block propagation protocol-related data structure (processing inventory)
        std::vector <std::string> _processingBlkinv;
        bool _processing = false;
        bool HasProcessingInv() { return _processing; }
        void StartProcessingInv() { _processing = true; }
        void StopProcessingInv() { _processing = false; }
        std::vector <std::string>& GetProcessingInv() { return _processingBlkinv; }
        void SetProcessingInv(std::vector <std::string> inv) { _processingBlkinv = inv; }


        /* handler functions for each asynchronous event */
        void RecvMsgHandler(std::shared_ptr<Message> msg);

        void _RecvPBFTBlockInvHandler(std::shared_ptr<Message> msg);
        void _RecvPBFTBlockGetBlocksHandler(std::shared_ptr<Message> msg);
        void _RecvPBFTBlockGetDataHandler(std::shared_ptr<Message> msg);
        void _RecvPBFTBlockBlkHandler(std::shared_ptr<Message> msg);

    private:
        std::shared_ptr<POSBlock> makeBlockTemplate();

        // periodic tx generation for experimental purpose
        ev::timer _txgentimer;
        void _txgentimerCallback(ev::timer &w, int revents) {
            // generate random transaction
            srand((unsigned int)time(0));
            int sender_id = rand() % 100;
            int receiver_id = rand() % 100;
            float amount = (float) (rand() % 100000);
            std::shared_ptr<SimpleTransaction> tx = std::make_shared<SimpleTransaction>(sender_id, receiver_id, amount);

            unsigned char hash_out[32];
            std::string sender_id_str = std::to_string(sender_id);
            SHA256_CTX ctx;
            sha256_init(&ctx);
            sha256_update(&ctx, (const unsigned char*)std::to_string(sender_id).c_str(), sizeof(int));
            sha256_update(&ctx, (const unsigned char*)std::to_string(receiver_id).c_str(), sizeof(int));
            sha256_update(&ctx, (const unsigned char*)std::to_string(amount).c_str(), sizeof(float));

            double timestamp = libBLEEP::GetGlobalClock();
            sha256_update(&ctx, (const unsigned char*)&timestamp, sizeof(double));
            sha256_final(&ctx, hash_out);
            libBLEEP::UINT256_t hash_out_256(hash_out, 32);
            tx->_id.SetTxHash(hash_out_256);
            std::cout<<"txgentimer generate new tx = "<<tx->_id.GetTxHash()<<"\n";

            memshare::try_share(tx);
            tx = memshare::lookup(tx);

            if (!_txPool->ContainTx(tx->GetId())) {
                _txPool->AddTx(tx);
                _txGossipProtocol.PushTxToBroadcast(tx);
            }
            _txGenNum += 1;
        }
        void _startPeriodicTxGen(double start, double interval) {
            _txgentimer.set<BL_ProtocolLayerPBFT, &BL_ProtocolLayerPBFT::_txgentimerCallback>(this);
            _txgentimer.set(start, interval);
            _txgentimer.start();
        }


    public:
        BL_ProtocolLayerPBFT();

        /* Switch asynchronous event to proper handler */
        virtual void SwitchAsyncEventHandler(AsyncEvent& event);

        /* public API functions */
        virtual bool InitiateProtocol();
        virtual bool InitiateProtocol(ProtocolParameter* params);
        virtual bool StopProtocol();
    };
}

#endif
