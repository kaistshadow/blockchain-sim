#ifndef STELLAR_CONSENSUS_DRIVER_H
#define STELLAR_CONSENSUS_DRIVER_H

#include <time.h>
#include "consensusdriver.h"

class StellarConsensusDriver : public ConsensusDriver {
 public:
    StellarConsensusDriver() {};

    
    bool HasNewConsensus();
    void UpdateLedger();

    bool PrepareNextConsensus() { return isReadyNow; };
    void TriggerNextConsensus();

 private:
    bool isReadyNow = true;
    time_t lastPrepareTime = 0;
};


#endif
