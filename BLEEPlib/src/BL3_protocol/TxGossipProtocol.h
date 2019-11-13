#ifndef TX_GOSSIP_PROTOCOL_H
#define TX_GOSSIP_PROTOCOL_H

#include <vector>
#include <iostream>
#include <boost/serialization/vector.hpp>

#include "TxPool.h"
#include "Transaction.h"
#include "../BL2_peer_connectivity/MessageObject.h"

namespace libBLEEP_BL {
    class Message;

    // TODO : make TxGossipMsgObj as templatized class using enum class ??
    // for example TxGossipMsgObj<enum::Inv>, TxGossipMsg<enum::GetData> ??
    class TxGossipInventory : public MessageObject {
    private:
        std::vector<SimpleTransactionId> _ids;
        
    public:
        TxGossipInventory() {}

        TxGossipInventory(std::vector<SimpleTransactionId> ids) { 
            for (auto id : ids) 
                _ids.push_back(id);
        }
        
        std::vector<SimpleTransactionId>& GetTransactionIds() { return _ids; }

    private: // boost serialization
        friend class boost::serialization::access;
        template<class Archive>
            void serialize(Archive & ar, const unsigned int version) {
            ar & boost::serialization::base_object<MessageObject>(*this);
            ar & _ids;
        }
    };

    class TxGossipGetdata : public MessageObject {
    private:
        std::vector<SimpleTransactionId> _ids;
        
    public:
        TxGossipGetdata() {}

        TxGossipGetdata(std::vector<SimpleTransactionId> ids) { 
            for (auto id : ids) 
                _ids.push_back(id);
        }
        std::vector<SimpleTransactionId>& GetTransactionIds() { return _ids; }

    private: // boost serialization
        friend class boost::serialization::access;
        template<class Archive>
            void serialize(Archive & ar, const unsigned int version) {
            ar & boost::serialization::base_object<MessageObject>(*this);
            ar & _ids;
        }
    };

    class TxGossipTxs : public MessageObject {
    private:
        std::vector<SimpleTransaction> _txs;
        
    public:
        TxGossipTxs() {}

        TxGossipTxs(std::vector<SimpleTransaction> &txs) { 
            for (auto tx : txs) 
                _txs.push_back(tx);
        }
        std::vector<SimpleTransaction>& GetTransactions() { return _txs; }

    private: // boost serialization
        friend class boost::serialization::access;
        template<class Archive>
            void serialize(Archive & ar, const unsigned int version) {
            ar & boost::serialization::base_object<MessageObject>(*this);
            ar & _txs;
        }
    };


    class TxGossipProtocol {
    private:
        std::shared_ptr<TxPool> _txPool;
        
    public:
        TxGossipProtocol(std::shared_ptr<TxPool> txPool) {_txPool = txPool;};
        
        void SetTxPool(std::shared_ptr<TxPool> txPool) { _txPool = txPool; }
        
        void RecvInventoryHandler(std::shared_ptr<Message> msg);

        void RecvGetdataHandler(std::shared_ptr<Message> msg);

        void RecvTxsHandler(std::shared_ptr<Message> msg);
    };
}


#endif
