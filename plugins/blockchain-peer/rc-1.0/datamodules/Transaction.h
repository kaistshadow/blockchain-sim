#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "../p2pnetwork/Message.h"


class Transaction: public Message {
 public:
    Transaction() {}
    Transaction(int sid, int rid, float a) {sender_id=sid; receiver_id=rid; amount=a; }
    virtual int GetType() {return TX_MESSAGE;}
    virtual ~Transaction() {}

    float GetAmount() { return amount;}

    int sender_id;
    int receiver_id;
    float amount;

 private:

    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
        boost::serialization::base_object<Message>(*this);
        boost::serialization::void_cast_register<Message,Transaction>();
        ar & sender_id;
        ar & receiver_id;
        ar & amount;
    }
};

std::ostream& operator<<(std::ostream&, const Transaction&); // in order to overload the << operator

bool operator<(Transaction const & lhs, Transaction const & rhs);
bool operator==(Transaction const & lhs, Transaction const & rhs);

#endif
