// "Copyright [2021] <kaistshadow>"

//
// Created by Yonggon Kim on 2021/04/25.
//

#ifndef BLEEPLIB_SRC_BL3_PROTOCOL_POWBLOCK_H_
#define BLEEPLIB_SRC_BL3_PROTOCOL_POWBLOCK_H_

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
class POWBlock : public Block {
 public:
    POWBlock() : Block() {}
    virtual ~POWBlock()
    { }

    POWBlock(std::string id, std::list<std::shared_ptr<SimpleTransaction> > input_tx_list)
            : Block(id, input_tx_list) {
        SetTxHash();
    }

    virtual void SetGenesisBlock() override;
    virtual void SetBlockHash(libBLEEP::UINT256_t hash) override { block_hash = hash; }
    virtual libBLEEP::UINT256_t GetBlockHash() const override { return block_hash; }

    libBLEEP::UINT256_t GetTxHash() const { return tx_hash; }

    void SetNonce(unsigned long n) { nonce = n; }
    unsigned long GetNonce() const {return nonce; }


    void SetPrevBlockHash(libBLEEP::UINT256_t hash) { prev_block_hash = hash; }
    libBLEEP::UINT256_t GetPrevBlockHash() const { return prev_block_hash; }

    void SetTimestamp(double t) { timestamp = t; }
    double GetTimestamp() const { return timestamp; }

    void SetDifficulty(libBLEEP::UINT256_t d) {difficulty = d; }
    libBLEEP::UINT256_t GetDifficulty() const { return difficulty; }

    virtual std::ostream& print(std::ostream& out) const override;

    std::size_t hash() {
        return (std::hash<std::string>()(block_hash.str()));
    }
    bool operator==(const POWBlock& other) {
        return nonce == other.nonce
            && tx_hash == other.tx_hash
            && prev_block_hash == other.prev_block_hash
            && timestamp == other.timestamp
            && difficulty == other.difficulty;
    }

 private:
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Block>(*this);
        boost::serialization::void_cast_register<Block,POWBlock>();
        ar & nonce;
        ar & tx_hash;
        ar & prev_block_hash;
        ar & timestamp;
        ar & difficulty;
    }

    void SetTxHash();
    unsigned long nonce;
    libBLEEP::UINT256_t tx_hash;


    libBLEEP::UINT256_t prev_block_hash;

    double timestamp = 0;
    libBLEEP::UINT256_t difficulty = 0;  // Currently, it's just threshold value. (unlike real bitcoin formula)
};
} // namespace libBLEEP_BL

#endif // BLEEPLIB_SRC_BL3_PROTOCOL_POWBLOCK_H_
