#ifndef PEER_CONNECTIVITY_LAYER_API_H
#define PEER_CONNECTIVITY_LAYER_API_H

#include "Peer.h"
#include "Message.h"
#include "BL_MainEventManager.h"

namespace libBLEEP_BL {
    class BL_PeerConnectivityLayer_API {
    public:
        BL_PeerConnectivityLayer_API() {};
        
        ~BL_PeerConnectivityLayer_API() {};
        
        /* Switch asynchronous event to proper handler */
        virtual void SwitchAsyncEventHandler(AsyncEvent& event) = 0;


        /* public API functions */
        virtual bool ConnectPeer(PeerId id) = 0;
        virtual bool DisconnectPeer(PeerId id) = 0;
        virtual bool SendMsgToPeer(PeerId id, std::shared_ptr<Message> msg) = 0; 
        virtual bool Shutdown() = 0;
        virtual std::vector<PeerId> GetNeighborPeerIds() = 0;

        virtual void StopOutgoingConnectionUpdate() = 0;
    };

    extern std::unique_ptr<BL_PeerConnectivityLayer_API> g_PeerConnectivityLayer_API;
}


#endif
