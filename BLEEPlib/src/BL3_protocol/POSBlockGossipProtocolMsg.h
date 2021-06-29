//
// Created by Hyunjin Kim on 2021/06/02.
//

#ifndef BLEEP_POSBLOCKGOSSIPPROTOCOLMSG_H
#define BLEEP_POSBLOCKGOSSIPPROTOCOLMSG_H

#include "../BL2_peer_connectivity/MessageObject.h"
#include "Transaction.h"
#include "POSBlock.h"

namespace libBLEEP_BL {
    class Message;

    class POSBlockGossipInventory : public MessageObject {
    private:
        std::vector <std::string> _vHash;

    public:
        POSBlockGossipInventory() {}

        POSBlockGossipInventory(std::vector<std::string> &hashes) {
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

    class POSBlockGossipGetBlocks : public MessageObject {
    private:
        std::vector <std::string> _blkLocator;

    public:
        POSBlockGossipGetBlocks() {}

        POSBlockGossipGetBlocks(std::vector<std::string> &hashes) {
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

    class POSBlockGossipGetData : public MessageObject {
    private:
        std::string _blkhash;

    public:
        POSBlockGossipGetData() {}

        POSBlockGossipGetData(std::string hash) {
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

    class POSBlockGossipBlk : public MessageObject {
    private:
        std::shared_ptr<POSBlock> _blk;

    public:
        POSBlockGossipBlk() {}

        POSBlockGossipBlk(std::shared_ptr<POSBlock> block) {
            _blk = block;
        }
        std::shared_ptr<POSBlock> &GetBlock() { return _blk; }

    private: // boost serialization
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & boost::serialization::base_object<MessageObject>(*this);
            ar & _blk;
        }
    };
}
#endif //BLEEP_POSBLOCKGOSSIPPROTOCOLMSG_H
