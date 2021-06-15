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
#include "BlockTree.h"
#include "BlockTree.cpp"  // This(BlockTree.cpp) is required since the BlockTree is template class
#include "PBFTConfig.h"
#include "PBFTSigning.h"
#include "PBFTMsgHolder.h"

#include "shadow_memshare_interface.h"

namespace libBLEEP_BL {
    class BL_ProtocolLayerPBFT : public BL_ProtocolLayer_API {
    private:
        TxGossipProtocol _txGossipProtocol;
        PBFTConfig _config;
        PBFTMsgHolder _msgs;
    private:
        BlockTree<PBFTBlock> _blocktree;
    public:
        BlockTree<PBFTBlock>& GetBlockTree() { return _blocktree; }
    private:
        // txgen parameter
        int maxTxPerBlock = 100;
        double txGenStartAt = 0;
        double txGenInterval = 4;

        // PBFT initial setup parameter
        std::string configFile = "config.txt";
        double phase_interval = 20;
        // PBFT logic parameter
        unsigned long _p;               // primary
        unsigned long _consensusNodeID = 0;
        unsigned long _v;               // current view#
        unsigned long _n;               // current seq#
        unsigned long _h;               // sequence boundary (bottom)
        unsigned long _k = 20;               // sequence range (_h + _k = H = Checkpoint starting sequence)  // TODO: parameterize
        unsigned long _f;               // faulty node limit
        // PBFT messages TODO
//        _preprepared;   // _prepreared msgs after _checkpoint
//        _prepared;      // _prepared msgs after _checkpoint
//        _checkpoints;    // > 2f+1 count of checkpoint msg with same seq# and digest.
//        _recentStableCheckpoint;
        PBFTSecret _secret;
        PBFTPubkey _pubkey;

    private:
        /* handler functions for each asynchronous event */
        void RecvMsgHandler(std::shared_ptr<Message> msg);
        
        void _RecvPBFTJoinRequestHandler(std::shared_ptr<Message> msg);
        void _RecvPBFTJoinResponseHandler(std::shared_ptr<Message> msg);
        void _StartPreprepare();
        void _RecvPBFTPreprepareHandler(std::shared_ptr<Message> msg);
        void _RecvPBFTPrepareHandler(std::shared_ptr<Message> msg);
        void _RecvPBFTCommitHandler(std::shared_ptr<Message> msg);
        void _changeViewCallback(ev::timer &w, int revents);
        void _RecvPBFTCheckpointHandler(std::shared_ptr<Message> msg);
        void _RecvPBFTViewChangeHandler(std::shared_ptr<Message> msg);

    private:
        std::shared_ptr<PBFTBlock> makeBlockTemplate();

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

        ev::timer _viewChangeStarter;
        double _viewChangeInterval = 100;       // TODO: make parameter input for view change interval setting
        void _initViewChangeStarter() {
            _viewChangeStarter.set<BL_ProtocolLayerPBFT, &BL_ProtocolLayerPBFT::_changeViewCallback>(this);
            _viewChangeStarter.set(0, _viewChangeInterval);
            _viewChangeStarter.start();
        }
        // called on new preprepare message
        void _renewViewChangeStarter() {
            // stop timer, reset timer with _viewChangeInterval
            _viewChangeStarter.stop();
            _viewChangeStarter.set(0, _viewChangeInterval);
            _viewChangeStarter.start();
        }

        ev::timer _joinTimer;
        void _joinTimerCallback(ev::timer &w, int revents);
        void _initJoinTimer() {
            _joinTimer.set<BL_ProtocolLayerPBFT, &BL_ProtocolLayerPBFT::_joinTimerCallback>(this);
            _joinTimer.set(5, 20);
            _joinTimer.start();
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
