#include "powconsensus.h"
#include "../crypto/sha256.h"
#include "../blockchain/txpool.h"

#include "../p2p/p2pmessage.h"
#include "../p2p/socket.h"
#include "../blockchain/powledgermanager.h"
#include "../p2p/gossipprotocol.h"

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

// void POWConsensus::BroadcastConsensus() {
//     // make consensus message (with valid ledger & PoW) & broadcast
// }

// Choose random nonce & calculate hash (i.e., proof of work)
//  to construct a valid block.
unsigned long POWConsensus::RunProofOfWork(POWBlock *pendingBlk, int trial) {

    // Calculate hash (i.e., proof of work) for the pending block
    for (int i = 0; i < trial; i++) {
        numTrial++;
        srand((unsigned int)time(0));
        unsigned long nonce = rand() * rand() * rand();
        unsigned char hash_out[32];

        SHA256_CTX ctx;
        sha256_init(&ctx);
        sha256_update(&ctx, (const unsigned char*)&nonce, sizeof(unsigned long));
        sha256_update(&ctx, (const unsigned char*)pendingBlk->GetTxHash().str().c_str(), pendingBlk->GetTxHash().str().size());
        sha256_final(&ctx, hash_out);

        utility::UINT256_t hash_out_256(hash_out, 32);
        unsigned char th[32] = {0x00,0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

        utility::UINT256_t threshold(th, 32);
        if (hash_out_256 < threshold) {
            pendingBlk->SetNonce(nonce);
            pendingBlk->SetBlockHash(hash_out_256);
            return nonce;
        }
    }

    return 0; // no valid nonce is found.
}

POWBlock *POWConsensus::Prepare() {
    // Data structure
    // pendingBlk : minumum requirement for proceeding consensus protocol. 
    POWBlock *pendingBlk = nullptr;

    // Build pending block (with pending transaction) if no pending block exists    

    if (TxPool::GetInstance()->GetPendingTxNum() >= POW_BLOCK_TX_NUM) {
        // if txpool has enough pending transactions

        std::list<Transaction> tx_list = TxPool::GetInstance()->GetTxs(POW_BLOCK_TX_NUM);
        pendingBlk = new POWBlock("pendingblk", tx_list);
    }
    else {
        // not enough pendingTx
        // std::cout << "Not enough pendingTx : " << TxPool::GetInstance()->GetPendingTxNum() << "\n";
    }

    return pendingBlk;
}

void POWConsensus::InjectValidBlockToP2PNetwork(POWBlock* pendingBlk) {
    POWConsensusMessage powmsg(POWConsensusMessage_NEWBLOCK, *pendingBlk, SimpleGossipProtocol::GetInstance()->GetHostId());

    P2PMessage p2pmessage(P2PMessage_POWCONSENSUSMESSAGE, powmsg);

    SimpleGossipProtocol::GetInstance()->PushToUpperQueue(p2pmessage);
    // SocketInterface::GetInstance()->UnicastP2PMsg(p2pmessage, "bleep1"); // hardcoded line for test
}

void POWConsensus::Run() {
    // main logic (state machine) for PoW Consensus protocol
    
    // Consensus Source : pending transactions
    //                    get txs from TxPool
    // Consensus Target : new valid block (i.e., set of transactions)
    //                    append a new block to a ledger using LedgerManager
    // Consensus Alg    : PoW

    POWBlock *pendingBlk = Prepare(); // prepare for consensus.
    if (pendingBlk == nullptr) 
        return;

    // Run consensus protocol
    // 1. Try to gain authorization to produce next block (PoW)
    // 2. If it succeed to make valid block, then try a consensus over P2P network. (propagate)
    // 3. There's no rule to finalize block. So, it immediately append a new valid block to a ledger.
    //    However, it follows longest chain rule. (final ledger can be modified later.)
    //    Thus, implement a PoW-specific ledger. (Tree & pointer to currently longest chain)
    
    // 1. authorization
    // calculate hash 1 time. 
    // (maybe  inefficient but to avoid fork)
    unsigned long nonce = RunProofOfWork(pendingBlk, 50); 
    if (!nonce) {
        return;
    }

    // 2. append a new valid block to a ledger. and propagate to network
    TxPool::GetInstance()->RemoveTxs(pendingBlk->GetTransactions());
    unsigned long blockidx = POWLedgerManager::GetInstance()->GetNextBlockIdx();
    pendingBlk->SetBlockIdx(blockidx);
    POWBlock *lastBlock = POWLedgerManager::GetInstance()->GetLastBlock();
    if (lastBlock) {
        pendingBlk->SetPrevBlockHash(lastBlock->GetBlockHash());
    }
    std::cout << "valid block found and appended" << "\n";
    std::cout << *pendingBlk << "\n";
    POWLedgerManager::GetInstance()->AppendBlock(*pendingBlk);
    InjectValidBlockToP2PNetwork(pendingBlk);

    delete pendingBlk;
}

void POWConsensus::ProcessQueue() {
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
                        std::cout << "txpool size:" << TxPool::GetInstance()->items.size() << "\n";
                        TxPool::GetInstance()->RemoveTxs(blk->GetTransactions());
                        std::cout << "after remove txpool size:" << TxPool::GetInstance()->items.size() << "\n";
                        POWLedgerManager::GetInstance()->AppendBlock(*blk);
                        std::cout << "Following block is received and appended" << "\n";
                        std::cout << *blk << "\n";
                    }
                    else if (lastblk->GetBlockHash() == blk->GetPrevBlockHash() && nextblkidx == blk->GetBlockIdx()) {
                        std::cout << "txpool size:" << TxPool::GetInstance()->items.size() << "\n";
                        TxPool::GetInstance()->RemoveTxs(blk->GetTransactions());
                        std::cout << "after remove txpool size:" << TxPool::GetInstance()->items.size() << "\n";
                        POWLedgerManager::GetInstance()->AppendBlock(*blk);
                        std::cout << "Following block is received and appended" << "\n";
                        std::cout << *blk << "\n";
                    }
                    else if (nextblkidx < blk->GetBlockIdx()) {
                        std::cout << "Block (sented by " << msg.msg_sender << ") is received and longer than mine" << "\n";
                        POWConsensusMessage powmsg(POWConsensusMessage_REQBLOCKS, SimpleGossipProtocol::GetInstance()->GetHostId());
                        UnicastMessage unimsg(UnicastMessage_POWCONSENSUSMESSAGE, powmsg);
    
                        SocketInterface::GetInstance()->SendUnicastMsg(unimsg, msg.msg_sender);
                    }
                    else 
                        std::cout << "Block is received but not appended" << "\n";
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
            }
            break;
        }
    }
}
