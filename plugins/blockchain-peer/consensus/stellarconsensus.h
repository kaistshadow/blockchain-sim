#ifndef CONSENSUS_STELLARCONSENSUS_H
#define CONSENSUS_STELLARCONSENSUS_H

#include <vector>
#include <queue>
#include <map>

#include "stellarconsensusvalue.h"
#include "stellarconsensusmessage.h"
#include "stellarquorum.h"

class StellarConsensus{
 private:
    StellarConsensus(){} // singleton pattern
    static StellarConsensus* instance; // singleton pattern

    std::queue<StellarConsensusMessage> msgQueue;

    bool CheckQuorumThreshold_nominate(StellarConsensusValue v); // 
    bool CheckQuorumThreshold_accept_nominate(StellarConsensusValue v); // 
    bool CheckBlockThreshold_nominate(StellarConsensusValue v); // 
    bool CheckBlockThreshold_accept_nominate(StellarConsensusValue v); // 
    void UpdateRoundLeader();
    bool CheckEcho(SCPNominate msg);

    StellarQuorums quorums;
    StellarQuorumSlices slices;
    std::string my_node_id;
    std::string round_leader = "";

    std::set<StellarConsensusValue> votedValues; 
    std::set<StellarConsensusValue> acceptedValues;
    std::set<StellarConsensusValue> candidateValues;
    std::map<std::string, SCPNominate> latestMessages; 
 public:
    static StellarConsensus* GetInstance(); // singleton pattern

    /**
     * Push message into message queue.
     */
    void PushToQueue(StellarConsensusMessage msg) { msgQueue.push(msg); }
    void ProcessQueue();
    std::string GetNodeId() {return my_node_id;}

};




#endif // CONSENSUS_STELLARCONSENSUS_H
