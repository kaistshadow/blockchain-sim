#ifndef STELLARQUORUM_H
#define STELLARQUORUM_H

#include <string>
#include <map>
#include <set>
#include <list>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/set.hpp>

// universal data structure which represents all quorums existing in the network
class StellarQuorums {
 public:
    StellarQuorums() {};
    void AddQuorum(std::set<std::string> q) { quorums.push_back(q); }
    
    std::list< std::set<std::string> > GetQuorums() const { return quorums; }
    bool inSameQuorum(std::string a, std::string b);
 private:
    std::list< std::set<std::string> > quorums;
    
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & quorums;
    }
};

// universal data structure which represents all quorumslices existing in the network
class StellarQuorumSlices {
 public:
    StellarQuorumSlices() {};  // TODO 1. init quorumslices
    void AddQuorumSlice(std::string node_id, std::set<std::string> slice) {
        quorumslices.insert(std::make_pair( node_id, slice));
    }

    std::map< std::string, std::set<std::string> > GetSlices() const { return quorumslices; }
 private:
    std::map< std::string, std::set<std::string> > quorumslices;

    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & quorumslices;
    }
};

std::ostream& operator<<(std::ostream&, const StellarQuorums&); // in order to overload the << operator
std::ostream& operator<<(std::ostream&, const StellarQuorumSlices&); // in order to overload the << operator

#endif // STELLARQUORUM_H
