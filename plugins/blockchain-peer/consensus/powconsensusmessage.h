#ifndef POWCONSENSUSMESSAGE_H
#define POWCONSENSUSMESSAGE_H

#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>

class POWConsensusMessage {
 public:
    POWConsensusMessage() {};

 private:
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
    }
    
};

#endif

