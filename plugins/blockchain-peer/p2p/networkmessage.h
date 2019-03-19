#ifndef NETWORK_MESSAGE_H
#define NETWORK_MESSAGE_H

#include <iostream>
#include <string>
#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/set.hpp>
#include <string>
#include "unicastmessage.h"
#include "p2pmessage.h"

#define NetworkMessage_P2PMSG 1
#define NetworkMessage_UNIMSG 2


class NetworkMessage {
 public:
  NetworkMessage() {};
  NetworkMessage(int t, P2PMessage msg) { type = t; data = msg; };
  NetworkMessage(int t, UnicastMessage msg) { type = t; data = msg; };

  int type;
  boost::variant<P2PMessage, UnicastMessage> data;

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
