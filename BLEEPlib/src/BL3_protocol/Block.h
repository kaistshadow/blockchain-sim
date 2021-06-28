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
#include "../utility/UInt256.h"

namespace libBLEEP_BL {
    class Block {
    public:
        Block() {};

        Block(std::string id, std::list<std::shared_ptr<SimpleTransaction> > input_tx_list) { block_id = id; tx_list = input_tx_list; }

        const std::list<std::shared_ptr<SimpleTransaction> >& GetTransactions() const { return tx_list; }
        std::string GetId() const {return block_id; }
        virtual void SetGenesisBlock() {}
        virtual void SetBlockHash(libBLEEP::UINT256_t hash) { block_hash = hash; }
        virtual libBLEEP::UINT256_t GetBlockHash() const { return block_hash; }

        void SetBlockIdx(unsigned long idx) {block_idx = idx; }
        unsigned long GetBlockIdx() const {return block_idx; }

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
            for (std::list<std::shared_ptr<SimpleTransaction> >::const_iterator it = tx_list.begin();
                 it != tx_list.end(); it++) {
                out << GetId() << ":" << *(*it) << "\n";
            }
            return out;
        }

    protected:
        std::string block_id;
        unsigned long block_idx;
        std::list<std::shared_ptr<SimpleTransaction> > tx_list;
        libBLEEP::UINT256_t block_hash;

    private:
        friend class boost::serialization::access;
        // When the class Archive corresponds to an output archive, the
        // & operator is defined similar to <<.  Likewise, when the class Archive
        // is a type of input archive the & operator is defined similar to >>

        template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
            ar & block_id;
            ar & block_idx;
            ar & tx_list;
            ar & block_hash;
        }
    };

}
#endif //BLEEP_BLOCK_H
