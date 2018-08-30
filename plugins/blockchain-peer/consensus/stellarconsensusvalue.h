#ifndef STELLARCONSENSUSVALUE_H
#define STELLARCONSENSUSVALUE_H

#include <list>
#include "../blockchain/transaction.h"

class StellarConsensusValue {
 public:
    StellarConsensusValue() {};

 private:
    std::list<Transaction> tx_list;

};

#endif
