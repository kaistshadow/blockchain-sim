#ifndef SIMPLECONSENSUSMESSAGE_H
#define SIMPLECONSENSUSMESSAGE_H

#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>


// In this file, we define a protocol messages required for simple centralized consensus.
// The simple centralized consensus is working with centralized supervisor node(i.e., injector)
// which selects a quorum and selects a leader of the quorum.


// assign a quorum to proceed a consensus algorithm.
// 'value' indicates a unique identifier assigned for each partipants of the quorum.
#define SimpleConsensusMessage_INIT_QUORUM 0   

// elect a leader among the quorum participants.
// 'value' indicates a unique identifier of the leader.
#define SimpleConsensusMessage_LEADER_ELECTION 1

class SimpleConsensusMessage {
 public:
    SimpleConsensusMessage() {};
    SimpleConsensusMessage(int t, int v) { type=t; value=v; };
    SimpleConsensusMessage(int t, std::string v) { type=t; value=v; };
    int type;
    boost::variant< int, std::string > value;

 private:
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & type;
        ar & value;
    }
    
};

#endif
