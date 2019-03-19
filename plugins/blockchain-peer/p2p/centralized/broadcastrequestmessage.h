#ifndef BROADCAST_REQUEST_MESSAGE_H
#define BROADCAST_REQUEST_MESSAGE_H

#include <iostream>
#include <string>
#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/set.hpp>
#include <set>
#include <string>


#include "../../blockchain/transaction.h"
#include "../../blockchain/block.h"
#include "../../consensus/simpleconsensusmessage.h"
/* #include "../../consensus/stellarconsensusmessage.h" */
#include "../../consensus/powconsensusmessage.h"

#define BroadcastRequestMessage_TRANSACTION 0
#define BroadcastRequestMessage_BLOCK 1
#define BroadcastRequestMessage_SIMPLECONSENSUSMESSAGE 2
/* #define BroadcastRequestMessage_STELLARCONSENSUSMESSAGE 3 */
#define BroadcastRequestMessage_POWCONSENSUSMESSAGE 4

class BroadcastRequestMessage {
 public:
  BroadcastRequestMessage() {};
  BroadcastRequestMessage(int t, Transaction x) {type = t; data = x; hop = 0;};
  BroadcastRequestMessage(int t, Block blk)     {type = t; data = blk; hop = 0;};
  BroadcastRequestMessage(int t, SimpleConsensusMessage msg) {type = t; data = msg; hop = 0;};
  /* BroadcastRequestMessage(int t, StellarConsensusMessage msg) { type = t; data = msg; hop = 0;}; */
  BroadcastRequestMessage(int t, POWConsensusMessage msg) { type = t; data = msg; hop = 0;};


  // variables for distintion 
  int type;
  boost::variant<Transaction, Block, SimpleConsensusMessage, POWConsensusMessage> data;

  // p2p traffic control (temporarily used for testing stellar consensus)
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
