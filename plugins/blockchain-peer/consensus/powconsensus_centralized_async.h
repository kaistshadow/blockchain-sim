#ifndef CONSENSUS_POWCONSENSUS_CENTRALIZED_ASYNC_H
#define CONSENSUS_POWCONSENSUS_CENTRALIZED_ASYNC_H

#include <vector>
#include <queue>
#include <map>
#include <ev.h>

#include "powconsensusmessage.h"
#include "../blockchain/powblock.h"

#define POW_BLOCK_TX_NUM 5

#define POW_EMULATED_MINING 1

#define POW_STATE_IDLE 0
#define POW_STATE_WAIT 1 // wait timer event for block generation

class POWConsensus {
 private:
    POWConsensus(){ ev_init (&mining_watcher, MiningCompleteCallback); } // singleton pattern
    static POWConsensus* instance; // singleton pattern

    std::queue<POWConsensusMessage> msgQueue;

    void InjectValidBlockToP2PNetwork(POWBlock *pendingBlk);
    void AppendBlockToLedgerAndPropagate(POWBlock *pendingBlk);


    /* int mining_option = 0; */
    int emulated_mining_state = 0;
    /* double emulated_mining_time = 0; */

    struct ev_loop *loop;
    ev_timer mining_watcher;
    static void MiningCompleteCallback(EV_P_ ev_timer *w, int revents);

 public:
    static POWConsensus* GetInstance(); // singleton pattern
    void SetEvLoop(struct ev_loop *l) { loop = l; }
    void SetMiningState(int state) { emulated_mining_state = state; }

    /**
     * Push message into message queue.
     */
    void PushToQueue(POWConsensusMessage msg) { msgQueue.push(msg); }
    bool ProcessQueue();
    void TriggerMiningEmulation();

};




#endif // CONSENSUS_POWCONSENSUS_CENTRALIZED_H
