#ifndef P2P_MESSAGE_H
#define P2P_MESSAGE_H

#include <iostream>
#include <boost/variant.hpp>

#include "../blockchain/transaction.h"
#include "../blockchain/block.h"

#define P2PMessage_TRANSACTION 0
/* enum P2PMessageType { */
/*     TRANSACTION = 0, */
/* }; */

class P2PMessage {
 public:
    P2PMessage() {};
    P2PMessage(int t, Transaction x) { type = t; data = x; };
    int type;
    boost::variant< Transaction, Block > data; // make it variant

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
