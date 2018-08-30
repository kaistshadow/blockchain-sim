#ifndef CONSENSUS_STELLARCONSENSUS_H
#define CONSENSUS_STELLARCONSENSUS_H

#include <vector>
#include <queue>

#include "stellarconsensusvalue.h"
#include "stellarconsensusmessage.h"
#include "stellarquorum.h"

class StellarConsensus{
 private:
    StellarConsensus(){} // singleton pattern
    static StellarConsensus* instance; // singleton pattern

    std::queue<StellarConsensusMessage> msgQueue;

    void CheckQuorumThreshold(StellarConsensusMessage msg); // 
    
    StellarQuorums quorums;
    StellarQuorumSlices slices;

    std::vector<StellarConsensusValue> votedValues;
    std::vector<StellarConsensusValue> acceptedValues;
    std::vector<StellarConsensusValue> candidateValues;
 public:
    static StellarConsensus* GetInstance(); // singleton pattern

    /**
     * Push message into message queue.
     */
    void PushToQueue(StellarConsensusMessage msg) { msgQueue.push(msg); }
    void ProcessQueue();

};




#endif // CONSENSUS_STELLARCONSENSUS_H
