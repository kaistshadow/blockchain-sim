#ifndef P2P_MESSAGE_H
#define P2P_MESSAGE_H

#include <iostream>
#include <string>
#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>
//#include <boost/serialization/string.hpp>

#include "../blockchain/transaction.h"
#include "../blockchain/block.h"
#include "../consensus/simpleconsensusmessage.h"
#include "membershipmessage.h"

#define P2PMessage_TRANSACTION 0
#define P2PMessage_BLOCK 1
#define P2PMessage_SIMPLECONSENSUSMESSAGE 2
#define P2PMessage_MEMBERSHIP 3

enum GOSSIP_TYPE {
  GOSSIP = 1,
  IHAVE  = 2,
  GRAFT  = 3,
  PRUNE  = 4
};

class P2PMessage {
 public:
  P2PMessage() {};
  P2PMessage(int t, Transaction x) {type = t; data = x; g_type = GOSSIP; g_round = 0;};
  P2PMessage(int t, Block blk)     {type = t; data = blk; g_type = GOSSIP; g_round = 0;};
  P2PMessage(int t, SimpleConsensusMessage msg) {type = t; data = msg; g_type = GOSSIP; g_round = 0;};
  P2PMessage(int t, MembershipMessage msg)      {type = t; data = msg; g_type = 0; g_round = 0;};
  // TODO : Add a msg type for gossip handling protocol that use IHAVE,GRAFT,PRUNE

  // variables for Gossip protocol PlumTree
  int g_type;
  int g_mid;
  int g_round;
  
  // variables for distintion 
  int type;
  boost::variant<Transaction, Block, SimpleConsensusMessage, MembershipMessage> data;

 private:
  friend class boost::serialization::access;
  // When the class Archive corresponds to an output archive, the
  // & operator is defined similar to <<.  Likewise, when the class Archive
  // is a type of input archive the & operator is defined similar to >>
    
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & g_type;
    ar & g_mid;
    ar & g_round;
    ar & type;
    ar & data;
  }
};

#endif
