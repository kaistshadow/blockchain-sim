
#include "HandleConsensus_POW.h"
#include "POWConsensusMessage.h"

#include "../Configuration.h"
#include "../utility/GlobalClock.h"
#include "../utility/NodeInfo.h"
#include "../event/GlobalEvent.h"
#include "../crypto/SHA256.h"

static void onMiningCompletionCallback(EV_P_ ev_timer *w, int revents) {
    dynamic_cast<HandleConsensus_POW*>(handleConsensusClass.get())->onMiningCompletion(EV_A_ w, revents);
}

HandleConsensus_POW::HandleConsensus_POW() {
    InitMiningTimer();
}

void HandleConsensus_POW::InitMiningTimer() {
    ev_init(&mining_timer, onMiningCompletionCallback);
    state = Idle;
}

void HandleConsensus_POW::onMiningCompletion(EV_P_ ev_timer *w, int revents) {
    // 1. calculate random block.
    srand((unsigned int)time(0));
    unsigned long nonce = rand() * rand() * rand();
    unsigned char hash_out[32];

    unsigned long blockidx = waitingBlk->GetBlockIdx();
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (const unsigned char*)&nonce, sizeof(unsigned long));
    sha256_update(&ctx, (const unsigned char*)waitingBlk->GetTxHash().str().c_str(), waitingBlk->GetTxHash().str().size());
    sha256_update(&ctx, (const unsigned char*)&blockidx, sizeof(unsigned long));
    sha256_update(&ctx, (const unsigned char*)waitingBlk->GetPrevBlockHash().str().c_str(), waitingBlk->GetPrevBlockHash().str().size());
    double timestamp = utility::GetGlobalClock();
    sha256_update(&ctx, (const unsigned char*)&timestamp, sizeof(double));
    sha256_final(&ctx, hash_out);

    utility::UINT256_t hash_out_256(hash_out, 32);
    waitingBlk->SetNonce(nonce);
    waitingBlk->SetBlockHash(hash_out_256);
    waitingBlk->SetTimestamp(timestamp);
                
    // 2. append a new valid block to a ledger. and propagate to network
    AppendBlockToLedger(waitingBlk);
    std::cout << "before broadcast" << "\n";
    POWConsensusMessage powmsg(POWConsensusMessage_NEWBLOCK, *waitingBlk, NodeInfo::GetInstance()->GetHostIP());
    handleNetworkClass->BroadcastMsg(&powmsg);
    // handleNetworkClass->BroadcastMsg(waitingBlk.get());
    std::cout << "after broadcast" << "\n";

    state = Idle;
    if (TxPool::GetInstance()->GetPendingTxNum() >= block_tx_num) {
        RequestConsensus(TxPool::GetInstance()->GetTxs(block_tx_num));
    }
}

void HandleConsensus_POW::AppendBlockToLedger(std::shared_ptr<POWBlock> blk) {
    TxPool::GetInstance()->RemoveTxs(blk->GetTransactions());
    std::cout << utility::GetGlobalClock() << ":valid block found and appended" << "\n";
    std::cout << *blk << "\n";
    ledger.AppendBlock(*blk);
}

void HandleConsensus_POW::RequestConsensus(std::list<Transaction> txs) {

    if (state != Idle)
        return;
    
    // Create candidate block 
    waitingBlk = std::shared_ptr<POWBlock>(new POWBlock("", txs));

    unsigned long blockidx = ledger.GetNextBlockIdx();
    waitingBlk->SetBlockIdx(blockidx);
    POWBlock *lastBlock = ledger.GetLastBlock();
    if (lastBlock) {
        waitingBlk->SetPrevBlockHash(lastBlock->GetBlockHash());
    }
    else {
        waitingBlk->SetPrevBlockHash(utility::UINT256_t(0xffffff)); // no valid prev block hash since it's genesis block
    }

    // trigger a new mining timer
    // by calculating the emulated mining time
    unsigned int random_num = time(0) * NodeInfo::GetInstance()->GetHostNumber();
    std::default_random_engine generator(random_num);
    std::normal_distribution<double> distribution(10.0, 2.0);
    double waiting_time = distribution(generator);
    if (waiting_time > 0) {
        double emulated_mining_time = utility::GetGlobalClock() + waiting_time;
        std::cout << utility::GetGlobalClock() << ":Set next emulated mining time:" << emulated_mining_time << "\n";
        state = MiningEventWaiting;

        // register mining timer
        // mining_watcher.repeat = waiting_time;
        // ev_timer_again(loop, &mining_watcher);
        ev_timer_set (&mining_timer, waiting_time, 0.);
        ev_timer_start (GlobalEvent::loop, &mining_timer);
    }
}

void HandleConsensus_POW::HandleArrivedConsensusMsg(ConsensusMessage* conmsg) {
    POWConsensusMessage* msg = dynamic_cast<POWConsensusMessage*>(conmsg);

    switch(msg->powmsgtype) {
    case POWConsensusMessage_NEWBLOCK:
        {
            POWBlock *blk = boost::get<POWBlock>(&msg->value);

            if (blk) {
                unsigned long nextblkidx = ledger.GetNextBlockIdx();
                POWBlock *lastblk = ledger.GetLastBlock(); 
                if (lastblk == nullptr || 
                    (lastblk->GetBlockHash() == blk->GetPrevBlockHash() && nextblkidx == blk->GetBlockIdx()) ) {
                    TxPool::GetInstance()->RemoveTxs(blk->GetTransactions());
                    ledger.AppendBlock(*blk);

                    std::cout << utility::GetGlobalClock() << ":Block is received and appended" << "\n";
                    std::cout << *blk << "\n";

                    state = Idle;
                    ev_timer_stop(GlobalEvent::loop, &mining_timer);
                    if (TxPool::GetInstance()->GetPendingTxNum() >= block_tx_num) {
                        RequestConsensus(TxPool::GetInstance()->GetTxs(block_tx_num));
                    }
                }
                else if (nextblkidx <= blk->GetBlockIdx()) {
                    std::cout << "Block (sented by " << msg->msg_sender << ") is received and longer than mine" << "\n";
                    POWConsensusMessage powmsg(POWConsensusMessage_REQBLOCKS, NodeInfo::GetInstance()->GetHostIP());
                    handleNetworkClass->UnicastMsg(msg->msg_sender, &powmsg);
                }
                else {
                    std::cout << utility::GetGlobalClock() << ":Block is received but not appended" << "\n";
                }
            }
            else {
                std::cout << "Wrong data in ConsensusMessage" << "\n";
                exit(1);
            } 
        }
        break;
    case POWConsensusMessage_REQBLOCKS: 
        {
            std::cout << "REQBLOCKS message is received from " << msg->msg_sender << "\n";
                
            POWBlocks blks;
            std::list<POWBlock>& ledger_blks = ledger.GetLedger();
            for (auto blk : ledger_blks) {
                blks.push_back(blk);
            }
            POWConsensusMessage powmsg(POWConsensusMessage_RESPBLOCKS, blks, NodeInfo::GetInstance()->GetHostIP());
            handleNetworkClass->UnicastMsg(msg->msg_sender, &powmsg);
        }
        break;
    case POWConsensusMessage_RESPBLOCKS: 
        {
            POWBlocks *blks = boost::get<POWBlocks>(&msg->value);

            std::cout << utility::GetGlobalClock() << ":RESPBLOCKS message is received from " << msg->msg_sender << "\n";

            if (ledger.UpdateLedgerAsLongestChain(blks)) {
                state = Idle;
                ev_timer_stop(GlobalEvent::loop, &mining_timer);
                if (TxPool::GetInstance()->GetPendingTxNum() >= block_tx_num) {
                    RequestConsensus(TxPool::GetInstance()->GetTxs(block_tx_num));
                }
            }
        }
        break;
    }
}
