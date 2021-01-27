#ifndef PEER_CONNECTIVITY_LAYER_API_H
#define PEER_CONNECTIVITY_LAYER_API_H

#include "Peer.h"
#include "Message.h"
#include "BL_MainEventManager.h"

#include "PeerManager.h"
#include "AddrManager.h"
#include "AdvertisementManager.h"
#include "../utility/Assert.h"

namespace libBLEEP_BL {
    class BL_PeerConnectivityLayer_API {

    public:
        static BL_PeerConnectivityLayer_API *Instance();
        static void InitInstance(std::string id);

    protected:
        BL_PeerConnectivityLayer_API(std::string id);

    private:
        static BL_PeerConnectivityLayer_API *_instance;

    public:
        ~BL_PeerConnectivityLayer_API() {};
        
        /* Switch asynchronous event to proper handler */
        virtual void SwitchAsyncEventHandler(AsyncEvent& event);


        /* public API functions */
        virtual bool ConnectPeer(PeerId id);
        virtual bool DisconnectPeer(PeerId id);
        virtual bool SendMsgToPeer(PeerId id, std::shared_ptr<Message> msg);
        virtual bool Shutdown();
        virtual std::vector<PeerId> GetNeighborPeerIds();


    private:
        PeerManager _peerManager;
        AddrManager _addrManager;
        AdvertisementManager _adManager;

        /* handler functions for each asynchronous event */
        void SocketConnectHandler(std::shared_ptr<DataSocket> dataSocket);
        void SocketCloseHandler(std::shared_ptr<DataSocket> closedSocket);
        void PeerNotifyHandler(PeerId incomingPeerId, std::shared_ptr<DataSocket> socket);
        void RecvMsgHandler(PeerId sourcePeerId, std::shared_ptr<Message> msg);

    private:
        // periodic outgoing connection update
        ev::timer _timer;
        void _timerCallback(ev::timer &w, int revents) {
            std::cout << "periodic outgoing connection update" << "\n";
            if (_peerManager.GetOutgoingPeerNum() < MAX_OUTGOINGPEER_NUM) {
                int trial = 0;
                while (trial++ < 10) {
                    std::shared_ptr<Address> addr = _addrManager.SelectAddressFromTable();
                    if (addr) {
                        PeerId peerId(addr->GetString());
                        std::shared_ptr<Peer> peer = _peerManager.FindPeer(peerId);
                        /* std::cout << "Addr:" << addr->GetString() << "\n"; */
                        if (peer && peer->IsActive())
                            continue;
                        else if (peer) {
                            std::cout << "connect to existing peer " << addr->GetString() << "\n";
                            // TODO : when it is happened? and what is the correct implementation?
                            libBLEEP::M_Assert(0, "to be implemented");
                            break;
                        }
                        else {
                            std::cout << "newly connect to peer " << addr->GetString() << "\n";
                            ConnectPeer(PeerId(addr->GetString()));
                            break;
                        }
                    }
                }
                _addrManager.PrintAddressTable();
            }
        }
        void _startOutgoingConnectionUpdateTimer(double time) {
            _timer.set<BL_PeerConnectivityLayer_API, &BL_PeerConnectivityLayer_API::_timerCallback>(this);
            _timer.set(time, time);
            _timer.start();
        }


    };
}


#endif
