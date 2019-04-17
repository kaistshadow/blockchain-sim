#ifndef POW_BLOCK_H
#define POW_BLOCK_H

#include <memory>
#include <iostream>
#include <string>
#include <list>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/list.hpp>
#include <boost/property_tree/ptree.hpp>


#include "../utility/UInt256.h"
#include "Block.h"
#include "Transaction.h"

namespace libBLEEP {
    class POWBlock : public Block {
    public:
        POWBlock() : Block() {};
        POWBlock(std::string id, std::list<boost::shared_ptr<Transaction> > input_tx_list) 
            : Block(id, input_tx_list) {
            SetTxHash();
        };

        UINT256_t GetTxHash() const { return tx_hash; }

        void SetNonce(unsigned long n) { nonce = n; }
        void SetBlockIdx(unsigned long idx) {block_idx = idx; }

        unsigned long GetNonce() const {return nonce; }
        unsigned long GetBlockIdx() const {return block_idx; }

        void SetBlockHash(UINT256_t hash) { block_hash = hash; }
        UINT256_t GetBlockHash() const { return block_hash; }

        void SetPrevBlockHash(UINT256_t hash) { prev_block_hash = hash; }
        UINT256_t GetPrevBlockHash() const { return prev_block_hash; }

        void SetTimestamp(double t) { timestamp = t; }
        double GetTimestamp() const { return timestamp; }

        void SetDifficulty(UINT256_t d) {difficulty = d; }
        UINT256_t GetDifficulty() const { return difficulty; }

        virtual std::ostream& print(std::ostream& out) const override; 
    private:
        friend class boost::serialization::access;
        // When the class Archive corresponds to an output archive, the
        // & operator is defined similar to <<.  Likewise, when the class Archive
        // is a type of input archive the & operator is defined similar to >>
        template<class Archive>
            void serialize(Archive & ar, const unsigned int version) {
            ar & boost::serialization::base_object<Block>(*this);
            boost::serialization::void_cast_register<Block,POWBlock>();
            ar & block_idx;
            ar & nonce;
            ar & tx_hash;
            ar & block_hash;
            ar & prev_block_hash;
            ar & timestamp;
            ar & difficulty;
        }

        void SetTxHash();
        unsigned long nonce;
        UINT256_t tx_hash;

        unsigned long block_idx;
        UINT256_t block_hash;
        UINT256_t prev_block_hash;

        double timestamp = 0;
        UINT256_t difficulty = 0;  // Currently, it's just threshold value. (unlike real bitcoin formula)

    };

    std::ostream& operator<<(std::ostream&, const POWBlock&); // in order to overload the << operator

}
#endif
