#ifndef PROTOCOL_LAYER_POW_H
#define PROTOCOL_LAYER_POW_H

#include "../BL_MainEventManager.h"
#include "../BL2_peer_connectivity/PeerConnectivityLayer_API.h"
#include "ProtocolLayer_API.h"
#include "Transaction.h"
#include "TxGossipProtocol.h"


namespace libBLEEP_BL {

    class BL_ProtocolLayerPoW : public BL_ProtocolLayer_API {
    private:

        /* handler functions for each asynchronous event */
        void RecvMsgHandler(std::shared_ptr<Message> msg);

    public:
        BL_ProtocolLayerPoW();

        /* Switch asynchronous event to proper handler */
        virtual void SwitchAsyncEventHandler(AsyncEvent& event);

        /* public API functions */
        virtual bool InitiateProtocol();
        virtual bool StopProtocol();
    };
}

#endif
