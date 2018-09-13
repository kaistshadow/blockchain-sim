#ifndef STELLARCONSENSUSMESSAGE_H
#define STELLARCONSENSUSMESSAGE_H

#include <string>
#include <vector>
#include "stellarquorum.h"
#include "stellarconsensusvalue.h"
#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/set.hpp>


#define StellarConsensusMessage_INIT_QUORUM 0   
#define StellarConsensusMessage_NOMINATE 1

struct SCPInit {
    StellarQuorums quorums;
    StellarQuorumSlices slices;
    std::string node_id;

 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & quorums;
        ar & slices;
        ar & node_id;
    }
};

struct SCPNominate {
    std::string sender_id;
    int slotIndex;
    std::set<StellarConsensusValue> voted;
    std::set<StellarConsensusValue> accepted;
 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & sender_id;
        ar & slotIndex;
        ar & voted;
        ar & accepted;
    }
};


class StellarConsensusMessage {
 public:
    StellarConsensusMessage() {};
    
    int type;
    boost::variant< SCPInit, SCPNominate > msg;

 private:
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & type;
        ar & msg;
    }
};



#endif
