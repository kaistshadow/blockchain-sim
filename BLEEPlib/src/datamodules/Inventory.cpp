#include "Inventory.h"

using namespace libBLEEP;

std::ostream& Inventory::print(std::ostream&out) const {
    out << "Inventory has following hashes" << "\n";
    const std::list<std::string>& hash_list = GetHashlist();
    for (std::list< std::string >::const_iterator it=hash_list.begin();
         it!=hash_list.end(); it++) {
        out << "Inventory" << ":" << (*it) << "\n";
    }
    return out;
}

