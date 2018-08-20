#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <list>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/list.hpp>

#include "transaction.h"

class Block {
 public:
    Block() {};
    Block(int index, std::list<Transaction> input_tx_list) { block_index = index; tx_list = input_tx_list; }

    const std::list<Transaction>& GetTransactions() const { return tx_list; }
    int GetIndex() const {return block_index; }

 private:
    int block_index;
    std::list<Transaction> tx_list;


    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    
    template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
        ar & block_index;
        ar & tx_list;
    }
};

std::ostream& operator<<(std::ostream&, const Block&); // in order to overload the << operator


#endif
