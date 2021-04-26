//
// Created by Yonggon Kim on 2021/04/25.
//

#ifndef BLEEP_BLOCK_H
#define BLEEP_BLOCK_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/list.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Transaction.h"

namespace libBLEEP_BL {
    class Block {
    public:
        Block() {};

        Block(std::string id, std::list<std::shared_ptr<SimpleTransaction> > input_tx_list) { block_id = id; tx_list = input_tx_list; }

        const std::list<std::shared_ptr<SimpleTransaction> >& GetTransactions() const { return tx_list; }
        std::string GetId() const {return block_id; }

        // This overloaded operator<<
        // Defines a non-member function, and makes it a friend of this class at the same time.
        friend std::ostream& operator<<(std::ostream &out, const Block &t)
        {
            return t.print(out);
        }
        // We'll rely on member function print() to do the actual printing
        // Because print is a normal member function, it can be virtualized
        virtual std::ostream& print(std::ostream&out) const  {
            out << GetId() << ":" << "Block has following transactions" << "\n";
            const std::list<std::shared_ptr<SimpleTransaction> > &tx_list = GetTransactions();
            for (std::list<std::shared_ptr<SimpleTransaction> >::const_iterator it = tx_list.begin();
                 it != tx_list.end(); it++) {
                out << GetId() << ":" << *(*it) << "\n";
            }
            return out;
        }

    protected:
        std::string block_id;
        std::list<std::shared_ptr<SimpleTransaction> > tx_list;

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

}
#endif //BLEEP_BLOCK_H
