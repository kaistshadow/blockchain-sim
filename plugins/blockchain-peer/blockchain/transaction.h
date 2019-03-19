#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <iostream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

class Transaction {
 public:
    Transaction() {};
    Transaction(int sid, int rid, float a) {sender_id=sid; receiver_id=rid; amount=a; }
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
        ar & sender_id;
        ar & receiver_id;
        ar & amount;
    }
};

std::ostream& operator<<(std::ostream&, const Transaction&); // in order to overload the << operator

bool operator<(Transaction const & lhs, Transaction const & rhs);
bool operator==(Transaction const & lhs, Transaction const & rhs);

#endif
