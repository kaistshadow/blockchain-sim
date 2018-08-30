#ifndef STELLARCONSENSUSMESSAGE_H
#define STELLARCONSENSUSMESSAGE_H

#include <vector>
#include "stellarquorum.h"
#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>


#define StellarConsensusMessage_INIT_QUORUM 0   
#define StellarConsensusMessage_NOMINATE 1

class SCPInit {
 public:
    SCPInit() {};

    StellarQuorums quorums;
    StellarQuorumSlices slices;
 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & quorums;
        ar & slices;
    }
};

class SCPNominate {
 public:
    SCPNominate() {};
    /* std::vector<StellarConsensusValue> voted; */
    /* std::vector<StellarConsensusValue> accepted; */
 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
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
