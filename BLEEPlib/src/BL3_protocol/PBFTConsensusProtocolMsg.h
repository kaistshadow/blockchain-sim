//
// Created by Hyunijn Kim on 2021/06/09.
//

#ifndef BLEEP_PBFTBLOCKCONSENSUSPROTOCOLMSG_H
#define BLEEP_PBFTBLOCKCONSENSUSPROTOCOLMSG_H

#include "../BL2_peer_connectivity/MessageObject.h"
#include "Transaction.h"
#include "PBFTBlock.h"

namespace libBLEEP_BL {
    class Message;

    class PBFTJoinRequest: public MessageObject {
    public:
        PBFTJoinRequest() {}
    private: // boost serialization
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & boost::serialization::base_object<MessageObject>(*this);
        }
    };
    class PBFTJoinResponse: public MessageObject {
    public:
        unsigned long consensusNodeId;
        std::string sign;
        PBFTJoinResponse() {}
        PBFTJoinResponse(unsigned long id, std::string sig) {
            consensusNodeId = id;
            sign = sig;
        }
    private: // boost serialization
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & boost::serialization::base_object<MessageObject>(*this);
            ar & consensusNodeId;
            ar & sign;
        }
    };
    class PBFTPreprepare : public MessageObject {
    private:
        unsigned long view;
        unsigned int sequence;
        std::string sign;
        std::shared_ptr<PBFTBlock> blk;
    public:
        PBFTPreprepare() {}
        PBFTPreprepare(unsigned long v, unsigned int n, std::string sig, std::shared_ptr<PBFTBlock> m) {
            view = v;
            sequence = n;
            sign = sig;
            blk = m;
        }
    private: // boost serialization
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & boost::serialization::base_object<MessageObject>(*this);
            ar & view;
            ar & sequence;
            ar & sign;
            ar & blk;
        }
    };

//    class Prepare : public MessageObject {
//
//    };
//    class Commit : public MessageObject {
//
//    }

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
#endif //BLEEP_PBFTBLOCKCONSENSUSPROTOCOLMSG_H
