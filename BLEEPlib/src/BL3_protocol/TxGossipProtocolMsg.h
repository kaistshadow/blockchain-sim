// "Copyright [2021] <kaistshadow>"

//
// Created by Yonggon Kim on 2021/05/01.
//

#ifndef BLEEP_TXGOSSIPPROTOCOLMSG_H
#define BLEEP_TXGOSSIPPROTOCOLMSG_H

#include "../BL2_peer_connectivity/MessageObject.h"
#include "Transaction.h"

namespace libBLEEP_BL {
    class Message;

    // TODO : make TxGossipMsgObj as templatized class using enum class ??
    // for example TxGossipMsgObj<enum::Inv>, TxGossipMsg<enum::GetData> ??
    class TxGossipInventory : public MessageObject {
    private:
        std::vector <SimpleTransactionId> _ids;

    public:
        TxGossipInventory() {}

        TxGossipInventory(std::vector <SimpleTransactionId> ids) {
            for (auto id : ids)
                _ids.push_back(id);
        }

        std::vector <SimpleTransactionId> &GetTransactionIds() { return _ids; }

    private: // boost serialization
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & boost::serialization::base_object<MessageObject>(*this);
            ar & _ids;
        }
    };

    class TxGossipGetdata : public MessageObject {
    private:
        std::vector <SimpleTransactionId> _ids;

    public:
        TxGossipGetdata() {}

        TxGossipGetdata(std::vector <SimpleTransactionId> ids) {
            for (auto id : ids)
                _ids.push_back(id);
        }

        std::vector <SimpleTransactionId> &GetTransactionIds() { return _ids; }

    private: // boost serialization
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & boost::serialization::base_object<MessageObject>(*this);
            ar & _ids;
        }
    };

    class TxGossipTxs : public MessageObject {
    private:
        std::vector <std::shared_ptr<SimpleTransaction>> _txs;

    public:
        TxGossipTxs() {}

        TxGossipTxs(std::vector <std::shared_ptr<SimpleTransaction>> &txs) {
            for (auto tx : txs)
                _txs.push_back(tx);
        }

        std::vector <std::shared_ptr<SimpleTransaction>> &GetTransactions() { return _txs; }

    private: // boost serialization
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & boost::serialization::base_object<MessageObject>(*this);
            ar & _txs;
        }
    };

}
#endif  // BLEEP_TXGOSSIPPROTOCOLMSG_H
