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
            std::shared_ptr<SimpleTransaction> tx = std::make_shared<SimpleTransaction>(sender_id, receiver_id, amount);
            if (!_txPool->ContainTx(tx->GetId())) {
                _txPool->AddTx(tx);
                _txGossipProtocol.PushTxToBroadcast(tx);
            }
        }
        void _startPeriodicTxGen(double start, double interval) {
            _txgentimer.set<BL_ProtocolLayerEx1, &BL_ProtocolLayerEx1::_txgentimerCallback>(this);
            _txgentimer.set(start, interval);
            _txgentimer.start();
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
