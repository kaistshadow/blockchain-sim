#ifndef BLOCK_H
#define BLOCK_H

#include <memory>
#include <iostream>
#include <string>
#include <list>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/list.hpp>
#include <boost/property_tree/ptree.hpp>


#include "Transaction.h"

class Block {
 public:
    Block() {};
    Block(std::string id, std::list<boost::shared_ptr<Transaction> > input_tx_list) { block_id = id; tx_list = input_tx_list; }

    const std::list<boost::shared_ptr<Transaction> >& GetTransactions() const { return tx_list; }
    std::string GetId() const {return block_id; }

 protected:
    std::string block_id;
    std::list<boost::shared_ptr<Transaction> > tx_list;

 private:
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    
    template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
        ar & block_id;
        ar & tx_list;
    }
};

std::ostream& operator<<(std::ostream&, const Block&); // in order to overload the << operator


#endif
