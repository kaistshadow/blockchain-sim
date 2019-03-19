#include "powconsensus_centralized_async.h"
#include "../crypto/sha256.h"
#include "../blockchain/txpool.h"

#include "../p2p/centralized/broadcastrequestmessage.h"
#include "../p2p/centralized/unicastrequestmessage.h"
#include "../p2p/centralized/asyncsocketinterface.h"
#include "../blockchain/powledgermanager.h"
#include "../util/globalclock.h"
#include "../util/hexstring.h"
#include "../util/logmanager.h"

#include "powconsensusmessage.h"

// Make a consensus using simple proof of work
// Consensus Source : pending transactions 
// Consensus Target : ledger (i.e., block) (i.e., set of transactions)
// Consensus Logic  : Periodic PoW


POWConsensus* POWConsensus::instance = 0;

POWConsensus* POWConsensus::GetInstance() {
    if (instance == 0) {
        instance = new POWConsensus();
    }
    return instance;
}


void POWConsensus::MiningCompleteCallback(EV_P_ ev_timer *w, int revents) {
    
    POWBlock *pendingBlk;
    std::list<Transaction> tx_list = TxPool::GetInstance()->GetTxs(POW_BLOCK_TX_NUM);
    pendingBlk = new POWBlock("pendingblk", tx_list);

    unsigned long blockidx = POWLedgerManager::GetInstance()->GetNextBlockIdx();
    pendingBlk->SetBlockIdx(blockidx);
    POWBlock *lastBlock = POWLedgerManager::GetInstance()->GetLastBlock();
    if (lastBlock) {
        pendingBlk->SetPrevBlockHash(lastBlock->GetBlockHash());
    }
    else {
        pendingBlk->SetPrevBlockHash(utility::UINT256_t(0xffffff)); // no valid prev block hash since it's genesis block
    }

    // 1. calculate random block.
    srand((unsigned int)time(0));
    unsigned long nonce = rand() * rand() * rand();
    unsigned char hash_out[32];

    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (const unsigned char*)&nonce, sizeof(unsigned long));
    sha256_update(&ctx, (const unsigned char*)pendingBlk->GetTxHash().str().c_str(), pendingBlk->GetTxHash().str().size());
    sha256_update(&ctx, (const unsigned char*)&blockidx, sizeof(unsigned long));
    sha256_update(&ctx, (const unsigned char*)pendingBlk->GetPrevBlockHash().str().c_str(), pendingBlk->GetPrevBlockHash().str().size());
    double timestamp = utility::GetGlobalClock();
    sha256_update(&ctx, (const unsigned char*)&timestamp, sizeof(double));
    sha256_final(&ctx, hash_out);

    utility::UINT256_t hash_out_256(hash_out, 32);
    pendingBlk->SetNonce(nonce);
    pendingBlk->SetBlockHash(hash_out_256);
    pendingBlk->SetTimestamp(timestamp);
                

    // 2. append a new valid block to a ledger. and propagate to network
    POWConsensus::GetInstance()->AppendBlockToLedgerAndPropagate(pendingBlk);
    delete pendingBlk;    
    
    POWConsensus::GetInstance()->SetMiningState(POW_STATE_IDLE);
    POWConsensus::GetInstance()->TriggerMiningEmulation();
}

void POWConsensus::InjectValidBlockToP2PNetwork(POWBlock* pendingBlk) {
    POWConsensusMessage powmsg(POWConsensusMessage_NEWBLOCK, *pendingBlk, NodeInfo::GetInstance()->GetHostId());

    BroadcastRequestMessage brmsg(BroadcastRequestMessage_POWCONSENSUSMESSAGE, powmsg);
    CentralizedNetworkMessage nmsg(CentralizedNetworkMessage_BROADCASTREQMSG, brmsg);

    AsyncSocketInterface::GetInstance()->SendNetworkMsg(nmsg, "bleep0"); // center is hardcoded to bleep0 for test
    std::cout << "POWConsensus: newblock broadcasting is requested" << "\n";
}

void POWConsensus::AppendBlockToLedgerAndPropagate(POWBlock *pendingBlk) {
    TxPool::GetInstance()->RemoveTxs(pendingBlk->GetTransactions());
    if (utility::GetLoggerOption() == LOGGER_ON) {
        std::string rawhash = pendingBlk->GetBlockHash().str().substr(0,6);
        std::string hashval = utility::HexStr(rawhash);
        std::cout << utility::GetGlobalClock() << ":valid block found and appended : " << hashval << "\n";
        std::cout << *pendingBlk << "\n";
    }
    else {
        std::cout << utility::GetGlobalClock() << ":valid block found and appended" << "\n";
        std::cout << *pendingBlk << "\n";
    }
    POWLedgerManager::GetInstance()->AppendBlock(*pendingBlk);
    POWLedgerManager::GetInstance()->DumpLedgerToJSONFile("ledger.json");
    InjectValidBlockToP2PNetwork(pendingBlk);
}

bool POWConsensus::ProcessQueue() {
    if (msgQueue.empty()) 
        return false;

    while (!msgQueue.empty()) {
        POWConsensusMessage msg = msgQueue.front();        
        msgQueue.pop();

        switch(msg.type) {
        case POWConsensusMessage_NEWBLOCK:
            {
                POWBlock *blk = boost::get<POWBlock>(&msg.value);
                if (blk) {
                    unsigned long nextblkidx = POWLedgerManager::GetInstance()->GetNextBlockIdx();
                    POWBlock *lastblk = POWLedgerManager::GetInstance()->GetLastBlock(); 
                    if (lastblk == nullptr) {
                        TxPool::GetInstance()->RemoveTxs(blk->GetTransactions());
                        POWLedgerManager::GetInstance()->AppendBlock(*blk);
                        POWLedgerManager::GetInstance()->DumpLedgerToJSONFile("ledger.json");

                        if (utility::GetLoggerOption() == LOGGER_ON) {
                            std::string rawhash = blk->GetBlockHash().str().substr(0,6);
                            std::string hashval = utility::HexStr(rawhash);
                            std::cout << utility::GetGlobalClock() << ":Block is received and appended :" << hashval << "\n";
                            std::cout << *blk << "\n";
                        }
                        else {
                            std::cout << utility::GetGlobalClock() << ":Block is received and appended" << "\n";
                            std::cout << *blk << "\n";
                        }
                        emulated_mining_state = POW_STATE_IDLE;
                        ev_timer_stop(loop, &mining_watcher);
                    }
                    else if (lastblk->GetBlockHash() == blk->GetPrevBlockHash() && nextblkidx == blk->GetBlockIdx()) {
                        TxPool::GetInstance()->RemoveTxs(blk->GetTransactions());
                        POWLedgerManager::GetInstance()->AppendBlock(*blk);
                        POWLedgerManager::GetInstance()->DumpLedgerToJSONFile("ledger.json");

                        if (utility::GetLoggerOption() == LOGGER_ON) {
                            std::string rawhash = blk->GetBlockHash().str().substr(0,6);
                            std::string hashval = utility::HexStr(rawhash);
                            std::cout << utility::GetGlobalClock() << ":Block is received and appended :" << hashval << "\n";
                            std::cout << *blk << "\n";
                        }
                        else {
                            std::cout << utility::GetGlobalClock() << ":Block is received and appended" << "\n";
                            std::cout << *blk << "\n";
                        }
                        emulated_mining_state = POW_STATE_IDLE;
                        ev_timer_stop(loop, &mining_watcher);
                    }
                    else if (nextblkidx <= blk->GetBlockIdx()) {
                        std::cout << "Block (sented by " << msg.msg_sender << ") is received and longer than mine" << "\n";
                        POWConsensusMessage powmsg(POWConsensusMessage_REQBLOCKS, NodeInfo::GetInstance()->GetHostId());
                        UnicastRequestMessage umsg(UnicastRequestMessage_POWCONSENSUSMESSAGE, powmsg, msg.msg_sender);
                        CentralizedNetworkMessage nmsg(CentralizedNetworkMessage_UNICASTREQMSG, umsg);

                        AsyncSocketInterface::GetInstance()->SendNetworkMsg(nmsg, "bleep0"); // center is hardcoded to bleep0 for test
                    }
                    else {
                        // cout << blk->GetBlockHash() << "\n";
                        if (utility::GetLoggerOption() == LOGGER_ON) {
                            std::string rawhash = blk->GetBlockHash().str().substr(0,6);
                            std::string hashval = utility::HexStr(rawhash);
                            std::cout << utility::GetGlobalClock() << ":Block is received but not appended :" << hashval << "\n";
                        }
                        else
                            std::cout << utility::GetGlobalClock() << ":Block is received but not appended" << "\n";
                    }
                }
                else {
                    std::cout << "Wrong data in P2PMessage" << "\n";
                    exit(1);
                } 
            }
            break;
        case POWConsensusMessage_REQBLOCKS: 
            {
                std::cout << "REQBLOCKS message is received from " << msg.msg_sender << "\n";
                
                POWBlocks blks;
                std::list<POWBlock>& ledger_blks = POWLedgerManager::GetInstance()->GetLedger();
                for (auto blk : ledger_blks) {
                    blks.push_back(blk);
                }
                POWConsensusMessage powmsg(POWConsensusMessage_RESPBLOCKS, blks, NodeInfo::GetInstance()->GetHostId());
                UnicastRequestMessage umsg(UnicastRequestMessage_POWCONSENSUSMESSAGE, powmsg, msg.msg_sender);
                CentralizedNetworkMessage nmsg(CentralizedNetworkMessage_UNICASTREQMSG, umsg);

                AsyncSocketInterface::GetInstance()->SendNetworkMsg(nmsg, "bleep0"); // center is hardcoded to bleep0 for test
            }
            break;
        case POWConsensusMessage_RESPBLOCKS: 
            {
                POWBlocks *blks = boost::get<POWBlocks>(&msg.value);
                if (utility::GetLoggerOption() == LOGGER_ON) {
                    std::string rawhash = blks->back().GetBlockHash().str().substr(0,6);
                    std::string hashval = utility::HexStr(rawhash);
                    std::cout << utility::GetGlobalClock() << ":RESPBLOCKS message is received from " << msg.msg_sender << ": last block hash=" << hashval << "\n";
                }
                else
                    std::cout << utility::GetGlobalClock() << ":RESPBLOCKS message is received from " << msg.msg_sender << "\n";

                if (POWLedgerManager::GetInstance()->UpdateLedgerAsLongestChain(blks)) {
                    emulated_mining_state = POW_STATE_IDLE;
                    ev_timer_stop(loop, &mining_watcher);
                }
            }
            break;
        }
    }
    return true;
}

void POWConsensus::TriggerMiningEmulation() {
    // If TxPool has enough pending transactions
    // and no mining timer is triggered yet (or triggered but stopped)
    // trigger a new mining timer
    if (TxPool::GetInstance()->GetPendingTxNum() >= POW_BLOCK_TX_NUM &&
        emulated_mining_state == POW_STATE_IDLE) {
        // calculate emulated mining time
        unsigned int random_num = time(0) * NodeInfo::GetInstance()->GetHostNumber();
        std::default_random_engine generator(random_num)
            ;            std::normal_distribution<double> distribution(10.0, 2.0);
        double waiting_time = distribution(generator);
        if (waiting_time > 0) {
            double emulated_mining_time = utility::GetGlobalClock() + waiting_time;
            std::cout << utility::GetGlobalClock() << ":Set next emulated mining time:" << emulated_mining_time << "\n";
            emulated_mining_state = POW_STATE_WAIT;

            // register mining timer
            // mining_watcher.repeat = waiting_time;
            // ev_timer_again(loop, &mining_watcher);
            ev_timer_set (&mining_watcher, waiting_time, 0.);
            ev_timer_start (loop, &mining_watcher);
        }
    }
}
