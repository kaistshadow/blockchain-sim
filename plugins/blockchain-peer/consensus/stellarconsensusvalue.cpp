#include "stellarconsensusvalue.h"

bool operator<(StellarConsensusValue const & lhs, StellarConsensusValue const & rhs) {
    if (lhs.tx_list.size() != rhs.tx_list.size())
        return lhs.tx_list.size() < rhs.tx_list.size();

    std::list<Transaction> new_lhs = lhs.tx_list;
    std::list<Transaction> new_rhs = rhs.tx_list;

    new_lhs.sort();
    new_rhs.sort();
    
    return (new_lhs < new_rhs);
}
bool operator==(StellarConsensusValue const & lhs, StellarConsensusValue const & rhs) {
    if (lhs.tx_list.size() != rhs.tx_list.size())
        return false;

    std::list<Transaction> new_lhs = lhs.tx_list;
    std::list<Transaction> new_rhs = rhs.tx_list;

    new_lhs.sort();
    new_rhs.sort();
    
    return (new_lhs == new_rhs);
}
