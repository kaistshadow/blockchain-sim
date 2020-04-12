#ifndef HANDLE_CONSENSUS_POW_H
#define HANDLE_CONSENSUS_POW_H

#include <ev.h>
#include "HandleConsensus.h"

#include "../datamanagermodules/POWLedgerManager.h"

#include "../event/Publisher.h"
#include "../event/Subscriber.h"

enum MiningState {
    Uninitialized,
    Idle,
    MiningEventWaiting
};

class HandleConsensus_POW: public HandleConsensus {
 private:
    // Event Publisher for mining emulation
    MiningEventEmulator miningEventEmulator;

    // Event Subscriber for miningCompleteEvent
    Subscriber miningCompleteEventSubscriber;

    // for mining status
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
    
    // callback function for subscribed event (miningCompleteEvent)
    void onMiningCompleteEvent(std::shared_ptr<EventInfo>);
};


#endif
