#ifndef POW_CONSENSUS_MESSAGE_H
#define POW_CONSENSUS_MESSAGE_H

#include "ConsensusMessage.h"
#include "../datamodules/POWBlock.h"

#include <iostream>
#include <string>
#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/vector.hpp>
#include <string>

// PoW Consensus Protocol Message 1. Broadcasting a newly generated block.
#define POWConsensusMessage_NEWBLOCK 1
// PoW Consensus Protocol Message 2. Request for Blocks
#define POWConsensusMessage_REQBLOCKS 2
// PoW Consensus Protocol Message 3. Response for Blocks
#define POWConsensusMessage_RESPBLOCKS 3

class POWConsensusMessage: public ConsensusMessage {
 public:
    POWConsensusMessage() {}
    POWConsensusMessage(int t, std::string sender) {powmsgtype = t; value = t; msg_sender = sender; };
    POWConsensusMessage(int t, POWBlock v, std::string sender) { powmsgtype = t; value = v; msg_sender = sender; };
    POWConsensusMessage(int t, POWBlocks v, std::string sender) { powmsgtype = t; value = v; msg_sender = sender; };

    virtual ~POWConsensusMessage() {}

    int powmsgtype;
    boost::variant< int, POWBlock, POWBlocks > value;
    std::string msg_sender;

 private:
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<ConsensusMessage>(*this);
        boost::serialization::void_cast_register<ConsensusMessage,POWConsensusMessage>();
        ar & powmsgtype;
        ar & value;
        ar & msg_sender;
    }
};

#endif
