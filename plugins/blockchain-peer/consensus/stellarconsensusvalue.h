#ifndef STELLARCONSENSUSVALUE_H
#define STELLARCONSENSUSVALUE_H

#include <list>
#include "../blockchain/transaction.h"

class StellarConsensusValue {
 public:
    StellarConsensusValue() {}
    StellarConsensusValue(std::list<Transaction> txs) { tx_list = txs; }

    friend bool operator== (StellarConsensusValue const & lhs, StellarConsensusValue const & rhs);
    friend bool operator< (StellarConsensusValue const & lhs, StellarConsensusValue const & rhs);

 private:
    std::list<Transaction> tx_list;

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & tx_list;
    }
};

bool operator<(StellarConsensusValue const & lhs, StellarConsensusValue const & rhs);
bool operator==(StellarConsensusValue const & lhs, StellarConsensusValue const & rhs);
#endif
