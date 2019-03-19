#ifndef CONSENSUS_SIMPLECONSENSUS_H
#define CONSENSUS_SIMPLECONSENSUS_H

#include <time.h>

#include <string>
#include <queue>

#include "simpleconsensusmessage.h"

class SimpleConsensus{ 
 private:
    SimpleConsensus(){}  // singleton pattern
    SimpleConsensus(bool isL){isLeader = isL; start_time = time(0); next_epoch_time = 1;}  // singleton pattern
    static SimpleConsensus* instance; // singleton pattern

    std::queue<SimpleConsensusMessage> msgQueue;
    std::string node_id;
    bool isLeader;
    time_t start_time;
    int next_epoch_time;
 public:
    static SimpleConsensus* GetInstance(); // singleton pattern

    /**
     * Simple consensus implementation
     * Leader (nodeid 0) periodically selects valid transactions and combines them as a block.
     */
    void RunConsensusProtocol();

    /** 
     * Push message into message queue.
     */
    void PushToQueue(SimpleConsensusMessage msg) { msgQueue.push(msg); }
    void ProcessQueue();
};

















#endif // P2P_SIMPLEPEERLIST_H
