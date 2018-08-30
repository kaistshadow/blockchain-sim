#include "stellarquorum.h"

std::ostream& operator<<(std::ostream& os, const StellarQuorums& quorums) {
    const std::list< std::set<std::string> >& q_list = quorums.GetQuorums();
    for (auto it = q_list.begin(); it != q_list.end(); it++) {
        std::set<std::string> slice = *it;        
        os << "Quorums:";

        // for (std::string str : slice) {
        for (auto str_it = slice.begin(); str_it != slice.end(); str_it++) {
            os << *str_it << " ";
        }
        os << "\n";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const StellarQuorumSlices& slices) {
    const std::map< std::string, std::set<std::string> >& q_slices = slices.GetSlices();
    for (auto it = q_slices.begin(); it != q_slices.end(); it++) {
        std::string node_id = it->first;
        std::set<std::string> slice = it->second;
        os << "QuorumSlices:";
        os << node_id << "'s slice is ";
        
        for (std::string str : slice) {
            os << str << " ";
        }
        os << "\n";
    }
    return os;
}

