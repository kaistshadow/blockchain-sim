#include "block.h"


std::ostream& operator<<(std::ostream& os, const Block& blk) {
    os << "== Block (idx:" << blk.GetIndex() << ") has following transactions ==" << "\n";
    const std::list<Transaction>& tx_list = blk.GetTransactions();
    for (std::list<Transaction>::const_iterator it=tx_list.begin();
         it!=tx_list.end(); it++) {
        os << *it << "\n";
    }
    os << "======================================";

    return os;
}
