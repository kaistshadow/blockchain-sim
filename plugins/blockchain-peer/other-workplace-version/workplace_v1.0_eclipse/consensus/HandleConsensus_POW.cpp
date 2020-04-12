
#include "HandleConsensus_POW.h"
#include "POWConsensusMessage.h"

#include "../Configuration.h"
#include "../utility/GlobalClock.h"
#include "../utility/NodeInfo.h"
#include "../crypto/SHA256.h"

HandleConsensus_POW::HandleConsensus_POW() : 
    miningEventEmulator(EventCoordinator::GetInstance(), mining_time, std::stof(mining_time_dev)),
    miningCompleteEventSubscriber(EventCoordinator::GetInstance(), 
                                  [this](std::shared_ptr<EventInfo> i){ onMiningCompleteEvent(i); },
                                  EventType::miningCompleteEvent)
    // refer to https://stackoverflow.com/a/402385 for understanding why i used lambda.
    // refer to https://stackoverflow.com/a/11284096 for understanding the way of using lambda in constructor.
                                  
{
    state = Idle;
}

void HandleConsensus_POW::onMiningCompleteEvent(std::shared_ptr<EventInfo> info) {
    state = Idle;
    miningEventEmulator.StopMiningTimer(); // there will be a case where the event does not come from emulator

    std::shared_ptr<MiningCompleteEventInfo> derivedInfo = std::static_pointer_cast<MiningCompleteEventInfo>(info);
    
    // 1. calculate random block.
    srand((unsigned int)time(0));
    unsigned long nonce = derivedInfo->GetNonce();
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

void HandleConsensus_POW::RequestConsensus(std::list<boost::shared_ptr<Transaction> > txs) {

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
    miningEventEmulator.StartMiningTimer();
    state = MiningEventWaiting;
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
                    miningEventEmulator.StopMiningTimer();
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
                miningEventEmulator.StopMiningTimer();
                if (TxPool::GetInstance()->GetPendingTxNum() >= block_tx_num) {
                    RequestConsensus(TxPool::GetInstance()->GetTxs(block_tx_num));
                }
            }
        }
        break;
    }
}
