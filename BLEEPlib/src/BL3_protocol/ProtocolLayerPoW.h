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

#include "shadow_memshare_interface.h"

namespace libBLEEP_BL {

    // PoW parameters
//    extern int txNumPerBlock;
//    extern double txGenStartAt;
//    extern double txGenInterval;
//    extern double miningtime;
//    extern int miningnodecnt;

    class OrphanElement {
    public:
        std::shared_ptr<POWBlockGossipInventory> inv;
        PeerId source;
        std::vector<std::string>::iterator current;

        OrphanElement(std::shared_ptr<POWBlockGossipInventory> inv, PeerId source, std::string hash) {
            this->inv = inv;
            this->source = source;

            auto it = this->inv->GetHashlist().begin();
            while (it != this->inv->GetHashlist().end()) {
                if (*it == hash) {
                    current = it;
                }
                it++;
            }
        }
        PeerId getSource() { return source; }
        std::string getCurrent() {
            return *current;
        }
        void proceed() {
            if (current != inv->GetHashlist().end()) {
                current++;
            }
        }
        std::shared_ptr<POWBlockGossipInventory> getInv() { return inv; }
    };


    class Orphanage {
        std::map<std::pair<std::string, std::string>, OrphanElement> invs;
        std::vector<std::shared_ptr<POWBlockGossipInventory>> solved_invs;
        std::vector<std::pair<PeerId, std::string>> next_request;

    public:
        Orphanage() {}
        int insertInv(std::shared_ptr<POWBlockGossipInventory> inv, PeerId peer, std::string hash){
            std::string front = inv->GetHashlist().front();
            std::string back = inv->GetHashlist().back();
            auto it = invs.find({front, back});
            if (it != invs.end()) {
                return 0;
            }
            invs.insert({{front, back}, OrphanElement(inv, peer, hash)});
            return 1;
        }
        void solveHash(std::string hash) {
            auto it = invs.begin();
            while (it != invs.end()) {
                if (it->first.second == hash) {
                    solved_invs.push_back(it->second.getInv());
                    auto prev_it = it;
                    it++;
                    invs.erase(prev_it);
                } else if(it->second.getCurrent() == hash) {
                    it->second.proceed();
                    next_request.push_back({it->second.getSource(), it->second.getCurrent()});
                    it++;
                } else {
                    it++;
                }
            }
        }
        std::vector<std::shared_ptr<POWBlockGossipInventory>> getSolvedInvs() { return solved_invs; }
        std::vector<std::pair<PeerId, std::string>> getNextRequest() { return next_request; }
        void clearResults() {
            solved_invs.clear();
            next_request.clear();
        }
    };

    class BL_ProtocolLayerPoW : public BL_ProtocolLayer_API {
    private:
        TxGossipProtocol _txGossipProtocol;
    private:
        POWMiner _powMiner;
        BlockTree<POWBlock> _blocktree;
        Orphanage orphanage;
    public:
        BlockTree<POWBlock>& GetBlockTree() { return _blocktree; }
    private: // PoW parameter
        int txNumPerBlock = 2;
        double txGenStartAt = 0;
        double txGenInterval = 4;
        double miningtime = 2;
        int miningnodecnt = 1;
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

        void _RecvPOWBlockInvHandler(std::shared_ptr<Message> msg);
        void _RecvPOWBlockGetBlocksHandler(std::shared_ptr<Message> msg);
        void _RecvPOWBlockGetDataHandler(std::shared_ptr<Message> msg);
        void _RecvPOWBlockBlkHandler(std::shared_ptr<Message> msg);

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

                if (_txPool->GetPendingTxNum() >= txNumPerBlock) {
                    if (!_powMiner.IsMining()) {
                        std::shared_ptr<POWBlock> candidateBlk = _makeCandidateBlock();
                        _powMiner.AsyncEmulateBlockMining(candidateBlk, 1/miningtime/miningnodecnt);
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
