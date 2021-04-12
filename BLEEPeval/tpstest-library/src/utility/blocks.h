//
// Created by csrc on 21. 4. 12..
//

#ifndef BLEEP_BLOCKS_H
#define BLEEP_BLOCKS_H

#include <map>
#include <queue>
#include <vector>
#include <string>

class block;
class blocktreebase;
class blockforest;

class block {
private:
    std::string hexHash;
    std::string hexPrevHash;
    uint32_t time;
    block* parent;
    blocktreebase* treebase;
    std::vector<std::string> txHashes;
public:
    block(std::string hexHash, std::string hexPrevHash, uint32_t time);
    std::string getHexHash();
    std::string getHexPrevHash();
    uint32_t getTime();
    size_t getTxCount();
    block* getParent();
    void setParent(block* bp);
    void pushTxHash(std::string txHash);
    void setTreebase(blocktreebase* tb);
    blocktreebase* getTreebase();
};
class blocktreebase {
private:
    block* besttip;
    int height;
    std::queue<block*> blocks;
public:
    blocktreebase(block* bp);
    void add_block(block* bp);
    void recalc_height_from_block(block* bp);
    int get_height();
    void merge_treebase(blocktreebase* btb);
    block* get_besttip();
};

class blockforest {
private:
    std::map<std::string, block*> blockmap;
    std::multimap<std::string, block*> blockchilds;
    int height;
    block* besttip;
public:
    blockforest();
    int add_block(block* bp);
    block* get_besttip();
};

#endif //BLEEP_BLOCKS_H
