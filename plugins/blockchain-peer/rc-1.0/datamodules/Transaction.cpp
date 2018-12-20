#include "Transaction.h"

std::ostream& operator<<(std::ostream& os, const Transaction& tx) {
    os << tx.sender_id << " sends " << tx.amount << " to " << tx.receiver_id ;

    return os;
}

bool operator<(Transaction const & lhs, Transaction const & rhs){
    if (lhs.sender_id == rhs.sender_id &&
             lhs.receiver_id == rhs.receiver_id)
        return lhs.amount < rhs.amount;
    else if (lhs.sender_id == rhs.sender_id)
        return lhs.receiver_id < rhs.receiver_id;
    else
        lhs.sender_id < rhs.sender_id;
}
bool operator==(Transaction const & lhs, Transaction const & rhs){
    if (lhs.sender_id == rhs.sender_id &&
        lhs.receiver_id == rhs.receiver_id &&
        lhs.amount == rhs.amount)
        return true;
    else
        return false;
}
