#include "transaction.h"


std::ostream& operator<<(std::ostream& os, const Transaction& tx) {
    os << tx.sender_id << " sends " << tx.amount << " to " << tx.receiver_id ;

    return os;
}
