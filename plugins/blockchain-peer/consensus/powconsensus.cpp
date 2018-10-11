#include "powconsensus.h"
#include "../crypto/sha256.h"
#include "../blockchain/txpool.h"

#include "../p2p/p2pmessage.h"
#include "../p2p/socket.h"
#include "../blockchain/powledgermanager.h"

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
        unsigned char th[32] = {0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

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
    P2PMessage p2pmessage(P2PMessage_POWBLOCK, *pendingBlk);

    SocketInterface::GetInstance()->UnicastP2PMsg(p2pmessage, "bleep1"); // hardcoded line for test
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
    //    Instead, it follows longest chain rule. (final ledger can be modified later.)
    //    Thus, implement a PoW-specific ledger. (Tree & pointer to currently longest chain)
    
    // 1. authorization
    // calculate hash 1 time. 
    // (maybe  inefficient but to avoid fork)
    unsigned long nonce = RunProofOfWork(pendingBlk, 1); 
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
}

void POWConsensus::ProcessQueue() {
    while (!msgQueue.empty()) {
        POWConsensusMessage msg = msgQueue.front();        
        msgQueue.pop();

        // if msg is consensus message, 
        // then remove consensed transactions from local pending requests
        // and wait for request of consensus driver.
    }
}
