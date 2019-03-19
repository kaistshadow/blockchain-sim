#ifndef PROXY_GENERATED_MESSAGE_H
#define PROXY_GENERATED_MESSAGE_H

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

#define ProxyGeneratedMessage_TRANSACTION 0
#define ProxyGeneratedMessage_BLOCK 1
#define ProxyGeneratedMessage_SIMPLECONSENSUSMESSAGE 2
/* #define ProxyGeneratedMessage_STELLARCONSENSUSMESSAGE 3 */
#define ProxyGeneratedMessage_POWCONSENSUSMESSAGE 4

class ProxyGeneratedMessage {
 public:
  ProxyGeneratedMessage() {};
  ProxyGeneratedMessage(int t, Transaction x) {type = t; data = x; };
  ProxyGeneratedMessage(int t, Block blk)     {type = t; data = blk; };
  ProxyGeneratedMessage(int t, SimpleConsensusMessage msg) {type = t; data = msg; };
  /* ProxyGeneratedMessage(int t, StellarConsensusMessage msg) { type = t; data = msg; hop = 0;}; */
  ProxyGeneratedMessage(int t, POWConsensusMessage msg) { type = t; data = msg; };


  // variables for distintion 
  int type;
  boost::variant<Transaction, Block, SimpleConsensusMessage, POWConsensusMessage> data;

 private:
  friend class boost::serialization::access;
  // When the class Archive corresponds to an output archive, the
  // & operator is defined similar to <<.  Likewise, when the class Archive
  // is a type of input archive the & operator is defined similar to >>
    
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & type;
    ar & data;
  }
};

#endif
