#include "jsonserializer.h"

void JSONSerializer::DumpToJson(boost::property_tree::ptree& root, POWBlock &blk) {
    boost::property_tree::ptree block_node;
    block_node.put("block_id", blk.GetId());
    for (auto &tx : blk.GetTransactions()) {
        boost::property_tree::ptree tx_node;
        tx_node.put("sender_id", tx.sender_id);
        tx_node.put("receiver_id", tx.receiver_id);
        tx_node.put("amount", tx.amount);
        block_node.add_child("transaction", tx_node);
    }
    block_node.put("block_idx", blk.GetBlockIdx());
    block_node.put("nonce", blk.GetNonce());
    block_node.put("tx_hash", blk.GetTxHash().str());
    block_node.put("block_hash", blk.GetBlockHash().str());
    block_node.put("prev_block_hash", blk.GetPrevBlockHash().str());

    root.add_child("block", block_node);
    return;
}

void JSONSerializer::DumpToJson(boost::property_tree::ptree& root, Block &blk) {
    boost::property_tree::ptree block_node;
    block_node.put("block_id", blk.GetId());
    for (auto &tx : blk.GetTransactions()) {
        boost::property_tree::ptree tx_node;
        tx_node.put("sender_id", tx.sender_id);
        tx_node.put("receiver_id", tx.receiver_id);
        tx_node.put("amount", tx.amount);
        block_node.add_child("transaction", tx_node);
    }
    root.add_child("block", block_node);
    return;
}
