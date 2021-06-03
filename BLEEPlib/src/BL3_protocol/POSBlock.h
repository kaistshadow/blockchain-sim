#ifndef BLEEP_POSBLOCK_H
#define BLEEP_POSBLOCK_H

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

namespace libBLEEP_BL {
    class POSBlock : public Block {
    private:
        libBLEEP::UINT256_t prev_block_hash;
        libBLEEP::UINT256_t tx_hash;
        unsigned int slot_no;
        signature sl_signature;
        
        
        int creator;
        unsigned int seed;          // for convenience
        double timestamp = 0;

        void SetTxHash();
    public:
        POSBlock() : Block() {}
        virtual ~POSBlock() {}
        POSBlock(int creator, std::string id, std::list<std::shared_ptr<SimpleTransaction> > input_tx_list)
                : Block(id, input_tx_list) {
            this->creator = creator;
            SetTxHash();
        };

        virtual void SetGenesisBlock() override;
        virtual std::ostream& print(std::ostream& out) const override;

        virtual void SetBlockHash(libBLEEP::UINT256_t hash) override { block_hash = hash; }
        virtual libBLEEP::UINT256_t GetBlockHash() const override { return block_hash; }
        libBLEEP::UINT256_t GetTxHash() const { return tx_hash; }
        void SetPrevBlockHash(libBLEEP::UINT256_t hash) { prev_block_hash = hash; }
        libBLEEP::UINT256_t GetPrevBlockHash() const { return prev_block_hash; }
        void SetTimestamp(double t) { timestamp = t; }
        double GetTimestamp() const { return timestamp; }

    private:
        friend class boost::serialization::access;
        // When the class Archive corresponds to an output archive, the
        // & operator is defined similar to <<.  Likewise, when the class Archive
        // is a type of input archive the & operator is defined similar to >>
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
            ar & boost::serialization::base_object<Block>(*this);
            boost::serialization::void_cast_register<Block,POSBlock>();
            ar & creator;
            ar & seed;
            ar & tx_hash;
            ar & prev_block_hash;
            ar & timestamp;
        }
    };
}
#endif //BLEEP_POSBLOCK_H
