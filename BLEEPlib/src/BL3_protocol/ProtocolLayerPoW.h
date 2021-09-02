// "Copyright [2021] <kaistshadow>"

#ifndef BLEEPLIB_SRC_BL3_PROTOCOL_PROTOCOLLAYERPOW_H_
#define BLEEPLIB_SRC_BL3_PROTOCOL_PROTOCOLLAYERPOW_H_

#include <vector>
#include <string>
#include <list>
#include <memory>
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
    // extern int txNumPerBlock;
    // extern double txGenStartAt;
    // extern double txGenInterval;
    // extern double miningtime;
    // extern int miningnodecnt;

class BL_ProtocolLayerPoW : public BL_ProtocolLayer_API {
 private:
    TxGossipProtocol _txGossipProtocol;

 private:
    POWMiner _powMiner;
    BlockTree<POWBlock> _blocktree;

 public:
    BlockTree<POWBlock>& GetBlockTree() { return _blocktree; }

 private:  // PoW parameter
    int txNumPerBlock = 2;
    double txGenStartAt = 0;
    double txGenInterval = 4;
    double miningtime = 2;
    int miningnodecnt = 1;

 private:  // Block propagation protocol-related data structure (processing inventory)
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
        std::cout << "txgentimer generate new tx = " << tx->_id.GetTxHash() << "\n";

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
}  // namespace libBLEEP_BL

#endif  // BLEEPLIB_SRC_BL3_PROTOCOL_PROTOCOLLAYERPOW_H_
