#ifndef PROTOCOL_LAYER_EX1_H
#define PROTOCOL_LAYER_EX1_H

#include "../BL_MainEventManager.h"
#include "ProtocolLayer_API.h"
#include "TxGossipProtocol.h"

namespace libBLEEP_BL {

    class BL_ProtocolLayerEx1 : public BL_ProtocolLayer_API {
    private:
        TxGossipProtocol _txGossipProtocol;

        /* handler functions for each asynchronous event */
        void RecvMsgHandler(std::shared_ptr<Message> msg);

    private:
        // periodic tx generation for experimental purpose        
        ev::timer _txgentimer;
        void _txgentimerCallback(ev::timer &w, int revents) {
            std::cout << "tx gen timer called" << "\n";
            std::vector<PeerId> neighborIds = g_PeerConnectivityLayer_API->GetNeighborPeerIds();
            for (auto neighborId : neighborIds) {
                std::cout << "send inv to " << neighborId.GetId() << "\n";
                std::shared_ptr<MessageObject> ptrToObj = std::make_shared<Inventory>();
                std::shared_ptr<Message> message = std::make_shared<Message>(neighborId, "TXGOSSIP-INV", ptrToObj);
                g_PeerConnectivityLayer_API->SendMsgToPeer(neighborId, message);
            }
        }
        void _startPeriodicTxGen(double start, double interval) {
            _txgentimer.set<BL_ProtocolLayerEx1, &BL_ProtocolLayerEx1::_txgentimerCallback>(this);
            _txgentimer.set(start, interval);
            _txgentimer.start();
        }

    public:
        BL_ProtocolLayerEx1() {};

        /* Switch asynchronous event to proper handler */
        virtual void SwitchAsyncEventHandler(AsyncEvent& event);

        /* public API functions */
        virtual bool InitiateProtocol();
        virtual bool StopProtocol();
    };
}

#endif
