// "Copyright [2021] <kaistshadow>"

//
// Created by Yonggon Kim on 2021/05/01.
//

#ifndef BLEEPLIB_SRC_BL3_PROTOCOL_POWBLOCKGOSSIPPROTOCOLMSG_H_
#define BLEEPLIB_SRC_BL3_PROTOCOL_POWBLOCKGOSSIPPROTOCOLMSG_H_

#include <string>
#include <memory>
#include <vector>
#include "../BL2_peer_connectivity/MessageObject.h"
#include "Transaction.h"
#include "POWBlock.h"

namespace libBLEEP_BL {
class Message;

class POWBlockGossipInventory : public MessageObject {
 private:
    std::vector <std::string> _vHash;

 public:
    POWBlockGossipInventory() {}

    POWBlockGossipInventory(std::vector<std::string> &hashes) {
        for (auto hash : hashes)
            _vHash.push_back(hash);
    }

    std::vector <std::string> &GetHashlist() { return _vHash; }

 private: // boost serialization
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & boost::serialization::base_object<MessageObject>(*this);
        ar & _vHash;
    }
};

class POWBlockGossipGetBlocks : public MessageObject {
 private:
    std::vector <std::string> _blkLocator;

 public:
    POWBlockGossipGetBlocks() {}

    POWBlockGossipGetBlocks(std::vector<std::string> &hashes) {
        for (auto hash : hashes)
            _blkLocator.push_back(hash);
    }

    std::vector <std::string> &GetBlockLocator() { return _blkLocator; }

 private: // boost serialization
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & boost::serialization::base_object<MessageObject>(*this);
        ar & _blkLocator;
    }
};

class POWBlockGossipGetData : public MessageObject {
 private:
    std::string _blkhash;

 public:
    POWBlockGossipGetData() {}

    POWBlockGossipGetData(std::string hash) {
        _blkhash = hash;
    }
    std::string &GetBlockHash() { return _blkhash; }

 private: // boost serialization
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & boost::serialization::base_object<MessageObject>(*this);
        ar & _blkhash;
    }
};

class POWBlockGossipBlk : public MessageObject {
 private:
    std::shared_ptr<POWBlock> _blk;

 public:
    POWBlockGossipBlk() {}

    POWBlockGossipBlk(std::shared_ptr<POWBlock> block) {
        _blk = block;
    }
    std::shared_ptr<POWBlock> &GetBlock() { return _blk; }

 private: // boost serialization
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & boost::serialization::base_object<MessageObject>(*this);
        ar & _blk;
    }
};
} // namespace libBLEEP_BL

#endif // BLEEPLIB_SRC_BL3_PROTOCOL_POWBLOCKGOSSIPPROTOCOLMSG_H_
