#ifndef HANDLE_CONSENSUS_POW_H
#define HANDLE_CONSENSUS_POW_H

#include <ev.h>
#include "HandleConsensus.h"

#include "../datamanagermodules/POWLedgerManager.h"

enum MiningState {
    Uninitialized,
    Idle,
    MiningEventWaiting
};

class HandleConsensus_POW: public HandleConsensus {
 private:

    // for mining emulation
    void InitMiningTimer();
    ev_timer mining_timer;
    MiningState state;
    std::shared_ptr<POWBlock> waitingBlk;
    
    // for ledger management
    POWLedgerManager ledger;
    void AppendBlockToLedger(std::shared_ptr<POWBlock> blk);

 public:
    HandleConsensus_POW();
    virtual ~HandleConsensus_POW() {}

    void RequestConsensus(std::list<boost::shared_ptr<Transaction> > txs);
    void HandleArrivedConsensusMsg(ConsensusMessage* msg);
    
    void onMiningCompletion(EV_P_ ev_timer *w, int revents);
};


#endif
