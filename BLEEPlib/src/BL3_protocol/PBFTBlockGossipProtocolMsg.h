//
// Created by Hyunijn Kim on 2021/06/09.
//

#ifndef BLEEP_PBFTBLOCKGOSSIPPROTOCOLMSG_H
#define BLEEP_PBFTBLOCKGOSSIPPROTOCOLMSG_H

#include "../BL2_peer_connectivity/MessageObject.h"
#include "Transaction.h"
#include "PBFTBlock.h"

namespace libBLEEP_BL {
    class Message;

    // TODO: not for consensus, since PBFT make consensus explicitly so that all consensus target block is unknown to the peer. Just send whole data, not inv.
    class PBFTBlockGossipInventory : public MessageObject {
    private:
        std::vector <std::string> _vHash;

    public:
        PBFTBlockGossipInventory() {}

        PBFTBlockGossipInventory(std::vector<std::string> &hashes) {
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

    // TODO: not for consensus, same reason with inv message above.
    class PBFTBlockGossipGetBlocks : public MessageObject {
    private:
        std::vector <std::string> _blkLocator;

    public:
        PBFTBlockGossipGetBlocks() {}

        PBFTBlockGossipGetBlocks(std::vector<std::string> &hashes) {
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

    // TODO: not for consensus, same reason with inv message above.
    class PBFTBlockGossipGetData : public MessageObject {
    private:
        std::string _blkhash;

    public:
        PBFTBlockGossipGetData() {}

        PBFTBlockGossipGetData(std::string hash) {
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

    class PBFTBlockGossipBlk : public MessageObject {
    private:
        std::shared_ptr<PBFTBlock> _blk;

    public:
        PBFTBlockGossipBlk() {}

        PBFTBlockGossipBlk(std::shared_ptr<PBFTBlock> block) {
            _blk = block;
        }
        std::shared_ptr<PBFTBlock> &GetBlock() { return _blk; }

    private: // boost serialization
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & boost::serialization::base_object<MessageObject>(*this);
            ar & _blk;
        }
    };
}
#endif //BLEEP_PBFTBLOCKGOSSIPPROTOCOLMSG_H
