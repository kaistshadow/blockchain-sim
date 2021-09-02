// "Copyright [2021] <kaistshadow>"

//
// Created by csrc on 21. 4. 12..
//

#ifndef BLEEPEVAL_PERFORMTEST_LIBRARY_SRC_UTILITY_BLOCKS_H_
#define BLEEPEVAL_PERFORMTEST_LIBRARY_SRC_UTILITY_BLOCKS_H_

#include <map>
#include <queue>
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>

class block;
class blocktreebase;
class blockforest;

class shared_timepool {
 private:
    std::unordered_map<std::string, uint32_t> txtimes;
    std::mutex _lock;

 public:
    void register_txtime(std::string txhash, uint32_t time);
    uint32_t get_txtime(std::string txhash);
};

extern shared_timepool* global_txtimepool;
class block {
 private:
    std::string hexHash;
    std::string hexPrevHash;
    uint32_t time;
    block* parent;
    blocktreebase* treebase;
    std::vector<std::string> txHashes;
    unsigned long int net_tx_latency;

 public:
    block(std::string hexHash, std::string hexPrevHash, uint32_t time);
    std::string getHexHash();
    std::string getHexPrevHash();
    uint32_t getTime();
    size_t getTxCount();
    block* getParent();
    void setParent(block* bp);
    void pushTxHash(std::string txHash);
    std::vector<std::string> getTxHashes();
    void setTreebase(blocktreebase* tb);
    blocktreebase* getTreebase();
    unsigned long int getNetTxLatency();
    void setNetTxLatency(unsigned long int netLatency);
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

blockforest bf;
blockforest get_blockforest() { return bf; }
bool UpdateBlock(std::string hexHash, std::string hexPrevHash, uint32_t time, std::vector<std::string> txs);

#endif  // BLEEPEVAL_PERFORMTEST_LIBRARY_SRC_UTILITY_BLOCKS_H_
