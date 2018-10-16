#ifndef POWCONSENSUSMESSAGE_H
#define POWCONSENSUSMESSAGE_H

#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>

#include "../blockchain/powblock.h"

// PoW Consensus Protocol Message 1. Broadcasting a newly generated block.
#define POWConsensusMessage_NEWBLOCK 1
// PoW Consensus Protocol Message 2. Request for Blocks
#define POWConsensusMessage_REQBLOCKS 2
// PoW Consensus Protocol Message 3. Response for Blocks

class POWConsensusMessage {
 public:
    POWConsensusMessage() {};
    POWConsensusMessage(int t, std::string sender) {type = t; msg_sender = sender; };
    POWConsensusMessage(int t, POWBlock v, std::string sender) { type = t; value = v; msg_sender = sender; };
    
    int type;
    boost::variant< POWBlock > value;
    std::string msg_sender;

 private:
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & type;
        ar & value;
        ar & msg_sender;
    }
    
};

#endif

