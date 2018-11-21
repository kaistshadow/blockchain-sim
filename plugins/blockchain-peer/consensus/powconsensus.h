#ifndef CONSENSUS_POWCONSENSUS_H
#define CONSENSUS_POWCONSENSUS_H

#include <vector>
#include <queue>
#include <map>

#include "powconsensusmessage.h"
#include "../blockchain/powblock.h"

#define POW_BLOCK_TX_NUM 5

#define POW_HASH_MINING 0
#define POW_EMULATED_MINING 1

#define POW_STATE_IDLE 0
#define POW_STATE_WAIT 1 // wait async event for block generation

class POWConsensus {
 private:
    POWConsensus(){} // singleton pattern
    static POWConsensus* instance; // singleton pattern

    std::queue<POWConsensusMessage> msgQueue;

    void InjectValidBlockToP2PNetwork(POWBlock *pendingBlk);
    void AppendBlockToLedgerAndPropagate(POWBlock *pendingBlk);

    bool RunProofOfWork(POWBlock *pendingBlk, int trial);
    bool RunEmulatedMining(POWBlock *pendingBlk);

    POWBlock* Prepare();
    POWBlock* pendingBlk = nullptr;
    unsigned long numTrial = 0;
    
    int mining_option = 0;
    int emulated_mining_state = 0;
    double emulated_mining_time = 0;

 public:
    static POWConsensus* GetInstance(); // singleton pattern

    void Run();

    /**
     * Push message into message queue.
     */
    void PushToQueue(POWConsensusMessage msg) { msgQueue.push(msg); }
    void ProcessQueue();

    void SetMiningOption(int opt) { mining_option = opt; }
};




#endif // CONSENSUS_POWCONSENSUS_H
