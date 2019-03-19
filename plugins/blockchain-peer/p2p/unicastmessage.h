#ifndef UNICAST_MESSAGE_H
#define UNICAST_MESSAGE_H

#include <iostream>
#include <string>
#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/set.hpp>
#include <set>
#include <string>

#include "../consensus/powconsensusmessage.h"

/* #define UnicastMessage_TRANSACTION 0 */
/* #define UnicastMessage_BLOCK 1 */
/* #define UnicastMessage_SIMPLECONSENSUSMESSAGE 2 */
/* #define UnicastMessage_STELLARCONSENSUSMESSAGE 3 */
#define UnicastMessage_POWCONSENSUSMESSAGE 4
/* #define UnicastMessage_MEMBERSHIP 5 */

class UnicastMessage {
 public:
  UnicastMessage() {};
  UnicastMessage(int t, POWConsensusMessage msg) { type = t; data = msg; };

  int type;
  boost::variant<POWConsensusMessage> data;

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
