//
// Created by Yonggon Kim on 2021/05/01.
//

#ifndef BLEEP_BLOCKTREE_H
#define BLEEP_BLOCKTREE_H

#include "Block.h"

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
        int _treeHeight = 0;
        UINT256_t _lasthash = 0;
        UINT256_t _genesis = 0;
    public:
        BlockTree() {}

        unsigned long GetNextBlockIdx() { return _treeheight; }
        UINT256_t GetLastHash() { return _lasthash; }

        std::vector<std::string> GetBlockLocator() {
            M_Assert(_lasthash != 0, "At least genesis block should exist");

            std::vector<std::string> blockLocator;


            if (_treeheight == 1) {
                blockLocator.push_back(_lasthash.str());
                return blockLocator;
            }

            std::shared_ptr<TreeBlockIndex<T> > blockIndex = _ledgermap[_lasthash.str()];
            while (true) {
                std::cout << "while in GetBlockLocator" << "\n";
                blockLocator.push_back(blockIndex->GetBlock()->GetBlockHash().str());

                if (blockLocator.size() >= 10) {
                    break;
                }
                else if (blockLocator.size() < 5) {
                    // get prevblock Index
                    std::string prevBlockHash = blockIndex->GetPrevBlockHash();
                    if (prevBlockHash == "") {
                        M_Assert ( _genesis == blockIndex->GetBlock()->GetBlockHash(), "it should be genesis block");
                        blockLocator.push_back(_genesis.str());
                        return blockLocator;
                    }

                    M_Assert(_ledgermap.find(prevBlockHash) != _ledgermap.end(), "ledger should contain prevBlockHash");
                    blockIndex = _ledgermap[prevBlockHash];
                }
                else {
                    // get blockIndex by walking up the tree (2, 4, 8, 16, 32 steps upward)
                    int step = 2;
                    for (int i = 0; i < blockLocator.size() - 5; i++)
                        step *= 2;

                    for (int i = 0; i < step; i++) {
                        std::cout << "i:" << i << "\n";
                        std::string prevBlockHash = blockIndex->GetPrevBlockHash();
                        if (_ledgermap.find(prevBlockHash) == _ledgermap.end()) {
                            M_Assert ( _genesis == blockIndex->GetBlock()->GetBlockHash(), "it should be genesis block");
                            blockLocator.push_back(_genesis.str());
                            return blockLocator;
                        }
                        blockIndex = _ledgermap[prevBlockHash];
                        M_Assert(blockIndex != nullptr, "blockIndex should exist!");
                    }
                }
            }
            blockLocator.push_back(_genesis.str());
            return blockLocator;
        }

        std::shared_ptr<T> GetBlock(std::string hash) {
            M_Assert(ContainBlock(hash), "should contain block");
            return _ledgermap[hash]->GetBlock();
        }

        bool ContainBlockHash(std::string hash) {
            auto it = _ledgermap.find(hash);
            if (it != _ledgermap.end())
                return true;
            else
                return false;
        }

        bool ContainBlock(std::string hash) {
            auto it = _ledgermap.find(hash);
            if (it != _ledgermap.end() && _ledgermap[hash] != nullptr)
                return true;
            else
                return false;
        }

        std::string FindCommonBlock(std::vector<std::string> locator) {
            std::shared_ptr<TreeBlockIndex<T> > blockIndex = _ledgermap[_lasthash.str()];

            while (true) {
                std::string blkhash = blockIndex->GetBlock()->GetBlockHash().str();
                std::cout << "while in FindCommonBlock:" << blkhash << "\n";

                if (std::find(locator.begin(), locator.end(), blkhash) != locator.end())
                    return blkhash;

                std::string prevBlockHash = blockIndex->GetPrevBlockHash();
                M_Assert(prevBlockHash != "", "need to be returned before reaching genesis block");

                M_Assert(_ledgermap.find(prevBlockHash) != _ledgermap.end(), "ledger should contain prevBlockHash");
                blockIndex = _ledgermap[prevBlockHash];
            }
        }

        std::vector<std::string> GetChainFromBlock(std::string start) {
            std::vector<std::string> chain;
            std::shared_ptr<TreeBlockIndex<T> > blockIndex = _ledgermap[_lasthash.str()];

            while (true) {
                std::string blkhash = blockIndex->GetBlock()->GetBlockHash().str();
                std::cout << "while in GetChainFromBlock:" << blkhash << "\n";

                chain.push_back(blkhash);

                if (blkhash == start)
                    break;

                std::string prevBlockHash = blockIndex->GetPrevBlockHash();
                M_Assert(prevBlockHash != "", "need to be returned before reaching genesis block");

                M_Assert(_ledgermap.find(prevBlockHash) != _ledgermap.end(), "ledger should contain prevBlockHash");
                blockIndex = _ledgermap[prevBlockHash];
            }
            return chain;
        }

        void AppendBlockHash(std::string hash) {
            if (_lasthash == 0) {
                M_Assert(0, "Currently, AppendBlockHash should not be called for genesis block");
                _ledgermap.insert(std::make_pair(hash, nullptr ) );
                std::cout << "first insert called" << "\n";
                std::cout << "hash:" << hash << "\n";
                _lasthash = UINT256_t((const unsigned char*)hash.c_str(), 32);
                _genesis = _lasthash;
                std::cout << "created hash:" << _lasthash << "\n";
                _treeheight++;
            }
            else {
                _ledgermap.insert(std::make_pair(hash, nullptr ));
            }
            /* else if (_ledgermap.find(hash) != _ledgermap.end()){ */
            /*     _ledgermap.insert(std::make_pair(hash, nullptr ));                 */
            /* } */

        }

        void AppendBlock(std::shared_ptr<T> blk) {
            std::cout << "appendblock called" << "\n";
            if (_lasthash == 0) {
                std::shared_ptr<TreeBlockIndex<T> > genesisIndex = std::make_shared< TreeBlockIndex<T> >(blk, "");
                std::cout << "first make_shared called" << "\n";
                _ledgermap.insert(std::make_pair(blk->GetBlockHash().str(), genesisIndex ) );
                std::cout << "first insert called" << "\n";
                _lasthash = blk->GetBlockHash();
                _genesis = _lasthash;
                _treeheight++;
            }
            else {
                std::string prevBlockHash = blk->GetPrevBlockHash().str();
                auto it = _ledgermap.find(prevBlockHash);
                if (it == _ledgermap.end())
                    std::cout << "no valid prev block hash for " << blk->GetPrevBlockHash();
                else {
                    std::shared_ptr<TreeBlockIndex<T> > blockIndex = std::make_shared< TreeBlockIndex<T> >(blk, prevBlockHash);
                    std::cout << "make_shared called" << "\n";

                    _ledgermap.insert(std::make_pair(blk->GetBlockHash().str(), blockIndex ));
                    std::cout << "insert called" << "\n";

                    auto it = _ledgermap.find(blk->GetBlockHash().str());
                    if (it == _ledgermap.end()) {
                        std::cout << "no element" << "\n";
                    } else {
                        std::cout << "has element" << "\n";
                    }
                    if (it != _ledgermap.end() && _ledgermap[blk->GetBlockHash().str()] != nullptr) {
                        std::cout << "insert succeeded" << "\n";
                        std::cout << "parent block:" << blk->GetPrevBlockHash() << "\n";
                        std::cout << "appended block:" << blk->GetBlockHash().str() << "\n";
                    }
                    else
                        std::cout << "insert failed" << "\n";


                    if (_lasthash == blk->GetPrevBlockHash()) {
                        _lasthash = blk->GetBlockHash();
                        _treeheight++;
                        std::cout << "append to tip:" << blk->GetBlockHash().str() << "\n";

                        std::cout << "before dynamic_pointer_cast" << "\n";
                        auto pow_blk = std::dynamic_pointer_cast<POWBlock>(blk);
                        if (pow_blk) {
                            // append shadow log
                            char buf[256];

                            std::stringstream str, str2;
                            std::ostringstream ss, ss2;
                            str << pow_blk->GetBlockHash();
                            ss << str.rdbuf();
                            std::string blockHash = ss.str();

                            str2 << pow_blk->GetPrevBlockHash();
                            ss2 << str2.rdbuf();
                            std::string prevBlockHash = ss2.str();
                            sprintf(buf, "BlockAppend,%lu,%s,%s,%f",
                                    pow_blk->GetBlockIdx(),
                                    blockHash.substr(2, 10).c_str(),
                                    prevBlockHash.substr(2,10).c_str(),
                                    pow_blk->GetTimestamp());
                            shadow_push_eventlog(buf);
                        }
                    } else if (_treeheight <= blk->GetBlockIdx()) {
                        _lasthash = blk->GetBlockHash();
                        _treeheight = blk->GetBlockIdx()+1;
                        std::cout << "tip changing:" << blk->GetBlockHash().str() << "\n";
                        auto pow_blk = std::dynamic_pointer_cast<POWBlock>(blk);
                        if (pow_blk) {
                            // append shadow log
                            char buf[256];

                            std::stringstream str, str2;
                            std::ostringstream ss, ss2;
                            str << pow_blk->GetBlockHash();
                            ss << str.rdbuf();
                            std::string blockHash = ss.str();

                            str2 << pow_blk->GetPrevBlockHash();
                            ss2 << str2.rdbuf();
                            std::string prevBlockHash = ss2.str();
                            sprintf(buf, "BlockAppend,%lu,%s,%s,%f",
                                    pow_blk->GetBlockIdx(),
                                    blockHash.substr(2, 10).c_str(),
                                    prevBlockHash.substr(2,10).c_str(),
                                    pow_blk->GetTimestamp());
                            shadow_push_eventlog(buf);
                        }
                    }
                }
            }
        }

    };
}
#endif //BLEEP_BLOCKTREE_H
