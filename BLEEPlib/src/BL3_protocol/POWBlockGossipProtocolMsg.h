//
// Created by Yonggon Kim on 2021/05/01.
//

#ifndef BLEEP_POWBLOCKGOSSIPPROTOCOLMSG_H
#define BLEEP_POWBLOCKGOSSIPPROTOCOLMSG_H

#include "../BL2_peer_connectivity/MessageObject.h"
#include "Transaction.h"

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
}
#endif //BLEEP_POWBLOCKGOSSIPPROTOCOLMSG_H
