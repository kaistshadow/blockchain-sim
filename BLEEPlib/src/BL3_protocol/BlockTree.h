// "Copyright [2021] <kaistshadow>"

//
// Created by Yonggon Kim on 2021/05/01.
//

#ifndef BLEEPLIB_SRC_BL3_PROTOCOL_BLOCKTREE_H_
#define BLEEPLIB_SRC_BL3_PROTOCOL_BLOCKTREE_H_

#include <vector>
#include <string>
#include <memory>
#include <type_traits>
#include "Block.h"
#include "../utility/UInt256.h"
#include <unordered_map>

namespace libBLEEP_BL {
template <typename T>
class BlockTreeBlockIndex {
    static_assert(std::is_base_of<Block, T>::value, "T must inherit from Block");

 private:
    std::string _prevblockhash;
    std::shared_ptr<T> _block;

 public:
    BlockTreeBlockIndex(std::shared_ptr<T> block, std::string ancestor_blockhash)
    { _block = block; _prevblockhash = ancestor_blockhash; }

    std::string GetPrevBlockHash() { return _prevblockhash; }
    std::shared_ptr<T> GetBlock() { return _block; }
};

template <typename T>
class BlockTree {
    static_assert(std::is_base_of<Block, T>::value, "T must inherit from Block");

 protected:
    std::unordered_map<std::string, std::shared_ptr< BlockTreeBlockIndex<T> > > _ledgermap;
    unsigned long _treeheight = 0;
    libBLEEP::UINT256_t _lasthash = 0;
    libBLEEP::UINT256_t _genesis = 0;

 public:
    BlockTree();

    unsigned long GetNextBlockIdx() { return _treeheight; }
    libBLEEP::UINT256_t GetLastHash() { return _lasthash; }

    std::vector<std::string> GetBlockLocator();

    std::shared_ptr<T> GetBlock(std::string hash);

    bool ContainBlockHash(std::string hash);

    bool ContainBlock(std::string hash);

    std::string FindCommonBlock(std::vector<std::string> locator);

    std::vector<std::string> GetChainFromBlock(std::string start);

    void AppendBlockHash(std::string hash);

    void AppendBlock(std::shared_ptr<T> blk);

    void PrintLongestChain();
};

} // namespace libBLEEP_BL

#endif // BLEEPLIB_SRC_BL3_PROTOCOL_BLOCKTREE_H_
