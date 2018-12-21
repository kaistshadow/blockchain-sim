#ifndef CONSENSUS_MESSAGE_H
#define CONSENSUS_MESSAGE_H

#include "../p2pnetwork/Message.h"

#include <iostream>
#include <string>
#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/set.hpp>
#include <string>

class ConsensusMessage: public Message {
 public:
    ConsensusMessage() {}
    virtual int GetType() {return CONSENSUS_MESSAGE;}

    virtual ~ConsensusMessage() {}

 private:
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Message>(*this);
        boost::serialization::void_cast_register<Message,ConsensusMessage>();
    }
};

#endif
