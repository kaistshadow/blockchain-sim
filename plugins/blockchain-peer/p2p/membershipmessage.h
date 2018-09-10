#ifndef MEMBERSHIPMESSAGE_H
#define MEMBERSHIPMESSAGE_H

#include <iostream>
#include <vector>
#include <string>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

typedef enum membership_type{
  P_JOIN, 
  P_JOINREPLY,
  P_DISCONNECT,
  P_FORWARDJOIN,
  P_FORWARDJOINREPLY,
  P_NEIGHBOR,
  P_NEIGHBORREPLY,
  P_SHUFFLE,
  P_SHUFFLEREPLY
} M_Type;

class MembershipMessage {
 public:
  MembershipMessage() {};
  MembershipMessage(int mtype, int numhop, int option, std::string src) {
    type = mtype;
    ttl  = numhop;
    opt  = option;
    src_peer = src;
  }
  int type;
  int ttl;
  int opt;
  std::string src_peer;
  std::vector<std::string> shuffle_list;
  
  std::string GetSrcPeer() {return src_peer;}

 private:
  friend class boost::serialization::access;
  // When the class Archive corresponds to an output archive, the
  // & operator is defined similar to <<.  Likewise, when the class Archive
  // is a type of input archive the & operator is defined similar to >>
    
  template<class Archive> 
  void serialize(Archive & ar, const unsigned int version) {
    ar & type;
    ar & ttl;
    ar & opt;
    ar & src_peer;
    ar & shuffle_list;
  }
};

#endif
