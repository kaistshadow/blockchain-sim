#ifndef POWBLOCK_H
#define POWBLOCK_H

#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/list.hpp>
#include <boost/property_tree/ptree.hpp>

#include "block.h"
#include "transaction.h"
#include "../util/types.h"

class POWBlock : public Block {
 public:
    POWBlock() : Block() {};
    POWBlock(std::string id, std::list<Transaction> input_tx_list) : Block(id, input_tx_list) {
        SetTxHash();
    };
    
    utility::UINT256_t GetTxHash() { return tx_hash; }

    void SetNonce(unsigned long n) { nonce = n; }
    void SetBlockIdx(unsigned long idx) {block_idx = idx; }

    unsigned long GetNonce() const {return nonce; }
    unsigned long GetBlockIdx() const {return block_idx; }

    void SetBlockHash(utility::UINT256_t hash) { block_hash = hash; }
    utility::UINT256_t GetBlockHash() const { return block_hash; }

    void SetPrevBlockHash(utility::UINT256_t hash) { prev_block_hash = hash; }
    utility::UINT256_t GetPrevBlockHash() const { return prev_block_hash; }

    void SetTimestamp(double t) { timestamp = t; }
    double GetTimestamp() const { return timestamp; }

    void SetDifficulty(utility::UINT256_t d) {difficulty = d; }
    utility::UINT256_t GetDifficulty() const { return difficulty; }

    // virtual function induces a segmentation fault of shadow simulator
    // I don't know why! Need to debug later
    // void DumpToJson(boost::property_tree::ptree& root) override;

 private:
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Block>(*this);
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
    utility::UINT256_t tx_hash;

    unsigned long block_idx;
    utility::UINT256_t block_hash;
    utility::UINT256_t prev_block_hash;

    double timestamp = 0;
    utility::UINT256_t difficulty = 0;  // Currently, it's just threshold value. (unlike real bitcoin formula)

    /* std::string block_id; */
    /* std::list<Transaction> tx_list; */
};

std::ostream& operator<<(std::ostream&, const POWBlock&); // in order to overload the << operator

typedef std::vector<POWBlock> POWBlocks;

#endif
