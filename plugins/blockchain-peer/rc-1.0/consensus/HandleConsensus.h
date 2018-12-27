#ifndef HANDLE_CONSENSUS_H
#define HANDLE_CONSENSUS_H

#include <memory>
#include <list>
#include "../datamodules/Transaction.h"
#include "../datamodules/Block.h"
#include "ConsensusMessage.h"

#define HANDLE_CONSENSUS_POW 0

class HandleConsensus {
 public:
    virtual ~HandleConsensus() {};
    static std::shared_ptr<HandleConsensus> create(const int type);

    virtual void RequestConsensus(std::list<boost::shared_ptr<Transaction> > txs) = 0;
    virtual void HandleArrivedConsensusMsg(ConsensusMessage* msg) = 0;
    
};


#endif // HANDLE_CONSENSUS_H
