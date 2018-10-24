#ifndef CONSENSUS_POWCONSENSUS_CENTRALIZED_H
#define CONSENSUS_POWCONSENSUS_CENTRALIZED_H

#include <vector>
#include <queue>
#include <map>

#include "powconsensusmessage.h"
#include "../blockchain/powblock.h"

#define POW_BLOCK_TX_NUM 5

class POWConsensus {
 private:
    POWConsensus(){} // singleton pattern
    static POWConsensus* instance; // singleton pattern

    std::queue<POWConsensusMessage> msgQueue;

    void InjectValidBlockToP2PNetwork(POWBlock *pendingBlk);
    unsigned long RunProofOfWork(POWBlock *pendingBlk, int trial);

    POWBlock* Prepare();
    POWBlock* pendingBlk = nullptr;
    unsigned long numTrial = 0;

 public:
    static POWConsensus* GetInstance(); // singleton pattern

    void Run();

    /**
     * Push message into message queue.
     */
    void PushToQueue(POWConsensusMessage msg) { msgQueue.push(msg); }
    void ProcessQueue();
};




#endif // CONSENSUS_POWCONSENSUS_CENTRALIZED_H
