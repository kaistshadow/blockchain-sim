#ifndef P2P_MESSAGE_H
#define P2P_MESSAGE_H

#include <iostream>
#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/set.hpp>
#include <set>
#include <string>

#include "../blockchain/transaction.h"
#include "../blockchain/block.h"
#include "../consensus/simpleconsensusmessage.h"
#include "../consensus/stellarconsensusmessage.h"

#define P2PMessage_TRANSACTION 0
#define P2PMessage_BLOCK 1
#define P2PMessage_SIMPLECONSENSUSMESSAGE 2
#define P2PMessage_STELLARCONSENSUSMESSAGE 3
/* enum P2PMessageType { */
/*     TRANSACTION = 0, */
/* }; */

class P2PMessage {
 public:
    P2PMessage() {};
    P2PMessage(int t, Transaction x) { type = t; data = x; hop = 0; };
    P2PMessage(int t, Block blk) { type = t; data = blk; hop = 0; };
    P2PMessage(int t, SimpleConsensusMessage msg) { type = t; data = msg; hop = 0; };
    P2PMessage(int t, StellarConsensusMessage msg) { type = t; data = msg; hop = 0;};
    int type;
    boost::variant< Transaction, Block, SimpleConsensusMessage, StellarConsensusMessage > data; // make it variant

    int GetHopCount() { return hop; }
    void IncreaseHopCount() { hop += 1; }
    
    bool IsProcessedByNode(std::string node_id) { return (visited_node.find(node_id) != visited_node.end()); }
    void SetProcessedByNode(std::string node_id) { visited_node.insert(node_id); }

 private:
    int hop; // total count of hops that the P2Pmessage have traversed
    std::set< std::string > visited_node; // set of visited node. Currently, temporarily implemented for testing stellar consensus 


    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & type;
        ar & data;
        ar & hop;
        ar & visited_node;
    }
};



#endif
