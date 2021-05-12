//
// Created by csrc on 21. 4. 12..
//

#include "blocks.h"

void shared_timepool::register_txtime(std::string txhash, uint32_t time) {
    _lock.lock();
    txtimes.insert({txhash, time});
    _lock.unlock();
}
uint32_t shared_timepool::get_txtime(std::string txhash) {
    _lock.lock();
    auto it = txtimes.find(txhash);
    _lock.unlock();
    if (it != txtimes.end()) {
        return it->second;
    }
    return 0;
}
shared_timepool* global_txtimepool = new shared_timepool();
block::block(std::string hexHash, std::string hexPrevHash, uint32_t time) {
    this->hexHash = hexHash;
    this->hexPrevHash = hexPrevHash;
    this->time = time;
    parent = nullptr;
    net_tx_latency = 0;
}
std::string block::getHexHash() {
    return hexHash;
}
std::string block::getHexPrevHash() {
    return hexPrevHash;
}
uint32_t block::getTime() {
    return time;
}
size_t block::getTxCount() {
    return txHashes.size();
}
block* block::getParent() {
    return parent;
}
void block::setParent(block* bp) {
    parent = bp;
}
void block::pushTxHash(std::string txHash) {
    txHashes.push_back(txHash);
}
std::vector<std::string> block::getTxHashes() {
    return txHashes;
}
void block::setTreebase(blocktreebase* tb) {
    treebase = tb;
}
blocktreebase* block::getTreebase() {
    return treebase;
}
unsigned long int block::getNetTxLatency() {
    return net_tx_latency;
}
void block::setNetTxLatency(unsigned long int netLatency) {
    net_tx_latency = netLatency;
}

blocktreebase::blocktreebase(block* bp) {
    besttip = nullptr;
    blocks.push(bp);
    bp->setTreebase(this);
    recalc_height_from_block(bp);
}
void blocktreebase::add_block(block* bp) {
    blocks.push(bp);
    bp->setTreebase(this);
    recalc_height_from_block(bp);
}
void blocktreebase::recalc_height_from_block(block* bp) {
    block* cur = bp;
    int cur_height = 0;
    while(cur->getParent()) {
        cur_height++;
        cur = cur->getParent();
    }
    if (cur_height > height || besttip == nullptr) {
        besttip = bp;
        height = cur_height;
    }
}
int blocktreebase::get_height() {
    return height;
}
void blocktreebase::merge_treebase(blocktreebase* btb) {
    // move all blocks in target treebase
    while (btb->blocks.empty()) {
        auto it = btb->blocks.front();
        it->setTreebase(this);
        blocks.push(it);
        btb->blocks.pop();
    }
    // recalc height;
    recalc_height_from_block(btb->besttip);
}
block* blocktreebase::get_besttip() {
    return besttip;
}


blockforest::blockforest() {
    height = -1;
    besttip = nullptr;
}
int blockforest::add_block(block* bp) {
    // check if the block already exists. if exists, do nothing
    auto it = blockmap.find(bp->getHexHash());
    if (it != blockmap.end()) {
        return 0;
    }

    // insert block to the blockmap
    blockmap.insert({bp->getHexHash(), bp});

    // attach the block if parent's tree exist
    it = blockmap.find(bp->getHexPrevHash());
    if (it != blockmap.end()) {
        bp->setParent(it->second);
        bp->getParent()->getTreebase()->add_block(bp);
    } else {
        // create new treebase
        bp->setParent(nullptr);
        new blocktreebase(bp);
        blockchilds.insert({bp->getHexPrevHash(), bp});
    }

    // merge if mergeable trees exist
    auto mult_it = blockchilds.equal_range(bp->getHexHash());
    it = mult_it.first;
    while (it != mult_it.second) {
        // merge treebase
        it->second->setParent(bp);
        blocktreebase* prev_treebase = it->second->getTreebase();
        bp->getTreebase()->merge_treebase(prev_treebase);
        // delete target treebase
        delete prev_treebase;
        it++;
    }

    // update besttip
    if (bp->getTreebase()->get_height() > height) {
        height = bp->getTreebase()->get_height();
        besttip = bp->getTreebase()->get_besttip();
    }

    return 1;
}
block* blockforest::get_besttip() {
    return besttip;
}