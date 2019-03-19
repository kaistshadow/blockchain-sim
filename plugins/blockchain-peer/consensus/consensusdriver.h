#ifndef CONSENSUS_DRIVER_H
#define CONSENSUS_DRIVER_H

/* interface class for driving consensus 
 * BlockchainManager controls consensus through the interface provided by consensus driver.
 * Thus, any consensus should implement this interface class.
 */

class ConsensusDriver {
 public:
    virtual ~ConsensusDriver(){
    }
    
    
    virtual bool HasNewConsensus() = 0;
    virtual void UpdateLedger() = 0;

    virtual bool PrepareNextConsensus() = 0;
    virtual void TriggerNextConsensus() = 0;
};


#endif
