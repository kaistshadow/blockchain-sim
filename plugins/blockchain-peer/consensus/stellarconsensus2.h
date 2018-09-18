#ifndef CONSENSUS_STELLARCONSENSUS2_H
#define CONSENSUS_STELLARCONSENSUS2_H

#include <vector>
#include <queue>
#include <map>

#include "stellarconsensusvalue.h"
#include "stellarconsensusmessage.h"
#include "stellarquorum.h"

class StellarConsensus2{
 private:
    StellarConsensus2(){} // singleton pattern
    static StellarConsensus2* instance; // singleton pattern

    std::queue<StellarConsensusMessage> msgQueue;

    StellarQuorums quorums;
    StellarQuorumSlices slices;
    std::string my_node_id;
    int nomination_round = 0;
    std::string round_leader = "";

    std::set<StellarConsensusValue> votedValues; 
    std::set<StellarConsensusValue> acceptedValues;
    std::set<StellarConsensusValue> candidateValues;
    std::map<std::string, SCPNominate> latestMessages; 
 public:
    static StellarConsensus2* GetInstance(); // singleton pattern

    void StartNominationProtocol();
    void ProceedNominationRound();

    /**
     * Push message into message queue.
     */
    void PushToQueue(StellarConsensusMessage msg) { msgQueue.push(msg); }
    void ProcessQueue();
    std::string GetNodeId() {return my_node_id;}

};




#endif // CONSENSUS_STELLARCONSENSUS2_H
