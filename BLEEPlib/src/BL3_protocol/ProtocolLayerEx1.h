#ifndef PROTOCOL_LAYER_EX1_H
#define PROTOCOL_LAYER_EX1_H

#include "../BL_MainEventManager.h"
#include "../BL2_peer_connectivity/PeerConnectivityLayer_API.h"
#include "ProtocolLayer_API.h"
#include "Transaction.h"
#include "TxGossipProtocol.h"


namespace libBLEEP_BL {

    class BL_ProtocolLayerEx1 : public BL_ProtocolLayer_API {
    private:
        std::shared_ptr<TxPool> _txPool;
        std::vector<SimpleTransaction> _txToBroadcast;
        TxGossipProtocol _txGossipProtocol;

        /* handler functions for each asynchronous event */
        void RecvMsgHandler(std::shared_ptr<Message> msg);

    private:
        // periodic tx generation for experimental purpose        
        ev::timer _txgentimer;
        void _txgentimerCallback(ev::timer &w, int revents) {
            // generate random transaction 
            srand((unsigned int)time(0));
            int sender_id = rand() % 100;
            int receiver_id = rand() % 100;
            float amount = (float) (rand() % 100000);
            SimpleTransaction tx(sender_id,receiver_id,amount);
            if (!_txPool->ContainTx(tx.GetId())) {
                _txPool->AddTx(boost::make_shared<SimpleTransaction>(tx));
                _txToBroadcast.push_back(tx);
            }
        }
        void _startPeriodicTxGen(double start, double interval) {
            _txgentimer.set<BL_ProtocolLayerEx1, &BL_ProtocolLayerEx1::_txgentimerCallback>(this);
            _txgentimer.set(start, interval);
            _txgentimer.start();
        }

    private:
        // periodic tx broadcast
        ev::timer _txbctimer;
        void _txbroadcasttimerCallback(ev::timer &w, int revents) {
            std::cout << "tx broadcast timer called" << "\n";
            std::cout << _txToBroadcast.size() << "\n";
            if (_txToBroadcast.empty())
                return;

            std::vector<SimpleTransactionId> txids;
            for (auto tx : _txToBroadcast)
                txids.push_back(tx.GetId());

            std::vector<PeerId> neighborIds = BL_PeerConnectivityLayer_API::Instance()->GetNeighborPeerIds();
            for (auto neighborId : neighborIds) {
                std::cout << "send inv to " << neighborId.GetId() << "\n";
                std::shared_ptr<MessageObject> ptrToObj = std::make_shared<TxGossipInventory>(txids);
                std::shared_ptr<Message> message = std::make_shared<Message>(neighborId, "TXGOSSIP-INV", ptrToObj);
                BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(neighborId, message);
            }
            _txToBroadcast.clear();
        }
        void _startPeriodicTxBroadcast(double start, double interval) {
            _txbctimer.set<BL_ProtocolLayerEx1, &BL_ProtocolLayerEx1::_txbroadcasttimerCallback>(this);
            _txbctimer.set(start, interval);
            _txbctimer.start();
        }

    public:
        BL_ProtocolLayerEx1();

        /* Switch asynchronous event to proper handler */
        virtual void SwitchAsyncEventHandler(AsyncEvent& event);

        /* public API functions */
        virtual bool InitiateProtocol();
        virtual bool StopProtocol();
    };
}

#endif
