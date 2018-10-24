#ifndef UNICAST_REQUEST_MESSAGE_H
#define UNICAST_REQUEST_MESSAGE_H

#include <iostream>
#include <string>
#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/set.hpp>
#include <set>

#include "../../blockchain/transaction.h"
#include "../../blockchain/block.h"
#include "../../consensus/simpleconsensusmessage.h"
#include "../../consensus/powconsensusmessage.h"

#define UnicastRequestMessage_TRANSACTION 0
#define UnicastRequestMessage_BLOCK 1
#define UnicastRequestMessage_SIMPLECONSENSUSMESSAGE 2
/* #define UnicastRequestMessage_STELLARCONSENSUSMESSAGE 3 */
#define UnicastRequestMessage_POWCONSENSUSMESSAGE 4
/* #define UnicastRequestMessage_MEMBERSHIP 5 */

class UnicastRequestMessage {
 public:
  UnicastRequestMessage() {};
  UnicastRequestMessage(int t, Transaction x, std::string s) {type = t; data = x; dest = s;};
  UnicastRequestMessage(int t, Block blk, std::string s) {type = t; data = blk; dest = s;};
  UnicastRequestMessage(int t, SimpleConsensusMessage msg, std::string s) {type = t; data = msg; dest = s;};
  UnicastRequestMessage(int t, POWConsensusMessage msg, std::string s) { type = t; data = msg; dest = s;};

  int type;
  boost::variant<Transaction, Block, SimpleConsensusMessage, POWConsensusMessage> data;
  std::string dest;

 private:
  friend class boost::serialization::access;
  // When the class Archive corresponds to an output archive, the
  // & operator is defined similar to <<.  Likewise, when the class Archive
  // is a type of input archive the & operator is defined similar to >>
    
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & type;
    ar & data;
    ar & dest;
  }
};

#endif
