#ifndef PEER_CONNECTIVITY_LAYER_H
#define PEER_CONNECTIVITY_LAYER_H

#include "../BL_MainEventManager.h"

#include "PeerConnectivityLayer_API.h"
#include "Peer.h"
#include "PeerManager.h"
#include "Message.h"


namespace libBLEEP_BL {

    class BL_PeerConnectivityLayer : public BL_PeerConnectivityLayer_API {
    private:
        PeerManager _peerManager;

        /* handler functions for each asynchronous event */
        void SocketConnectHandler(std::shared_ptr<DataSocket> dataSocket);
        void PeerNotifyHandler(PeerId incomingPeerId, std::shared_ptr<DataSocket> socket);
        void RecvMsgHandler(PeerId sourcePeerId, std::shared_ptr<Message> msg);

    public:
        BL_PeerConnectivityLayer(std::string myPeerId);

        /* Switch asynchronous event to proper handler */
        virtual void SwitchAsyncEventHandler(AsyncEvent& event);


        /* public API functions */
        virtual bool ConnectPeer(PeerId id);
        virtual bool DisconnectPeer(PeerId id);
        virtual bool SendMsgToPeer(PeerId id, std::shared_ptr<Message> msg); 
    };

}

#endif
