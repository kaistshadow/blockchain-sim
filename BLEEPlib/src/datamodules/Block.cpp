#include "Block.h"

using namespace libBLEEP;

std::ostream& Block::print(std::ostream&out) const {
    out << GetId() << ":" << "Block has following transactions" << "\n";
    const std::list<boost::shared_ptr<Transaction> >& tx_list = GetTransactions();
    for (std::list<boost::shared_ptr<Transaction> >::const_iterator it=tx_list.begin();
         it!=tx_list.end(); it++) {
        out << GetId() << ":" << *(*it) << "\n";
    }
    return out;
}

// std::ostream& libBLEEP::operator<<(std::ostream& os, const Block& blk) {
//     os << blk.GetId() << ":" << "Block has following transactions" << "\n";
//     const std::list<boost::shared_ptr<Transaction> >& tx_list = blk.GetTransactions();
//     for (std::list<boost::shared_ptr<Transaction> >::const_iterator it=tx_list.begin();
//          it!=tx_list.end(); it++) {
//         os << blk.GetId() << ":" << *(*it) << "\n";
//     }

//     return os;
// }
