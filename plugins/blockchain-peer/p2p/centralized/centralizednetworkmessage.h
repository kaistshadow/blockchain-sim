#ifndef NETWORK_MESSAGE_H
#define NETWORK_MESSAGE_H

#include <iostream>
#include <string>
#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/set.hpp>
#include <string>
#include "broadcastrequestmessage.h"
#include "unicastrequestmessage.h"
#include "proxygeneratedmessage.h"


#define CentralizedNetworkMessage_BROADCASTREQMSG 1
#define CentralizedNetworkMessage_UNICASTREQMSG 2
#define CentralizedNetworkMessage_PROXYGENERATEDMSG 3
#define CentralizedNetworkMessage_DOMAINNOTIFYMSG 4

typedef std::string Domain;

class CentralizedNetworkMessage {
 public:
  CentralizedNetworkMessage() {};
  CentralizedNetworkMessage(int t, BroadcastRequestMessage msg) { type = t; data = msg; };
  CentralizedNetworkMessage(int t, UnicastRequestMessage msg) { type = t; data = msg; };
  CentralizedNetworkMessage(int t, ProxyGeneratedMessage msg) { type = t; data = msg; };
  CentralizedNetworkMessage(int t, Domain msg) { type = t; data = msg; };

  int type;
boost::variant<BroadcastRequestMessage, UnicastRequestMessage, ProxyGeneratedMessage, Domain> data;

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
