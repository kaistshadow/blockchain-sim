// "Copyright [2021] <kaistshadow>"

#ifndef BLEEPLIB_SRC_BL2_PEER_CONNECTIVITY_PEERCONNECTIVITYLAYER_API_H_
#define BLEEPLIB_SRC_BL2_PEER_CONNECTIVITY_PEERCONNECTIVITYLAYER_API_H_

#include <vector>
#include <string>
#include <memory>
#include "Peer.h"
#include "Message.h"
#include "BL_MainEventManager.h"

#include "PeerManager.h"
#include "AddrManager.h"
#include "AdvertisementManager.h"
#include "../utility/Assert.h"

#define MAX_OUTGOINGPEER_NUM 8
#define MAX_INCOMINGPEER_NUM 10

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
    ~BL_PeerConnectivityLayer_API() {}
    /* Switch asynchronous event to proper handler */
    virtual void SwitchAsyncEventHandler(AsyncEvent& event);


    /* public API functions */
    virtual bool ConnectPeer(PeerId id);
    virtual bool DisconnectPeer(PeerId id);
    virtual bool SendMsgToPeer(PeerId id, std::shared_ptr<Message> msg);
    virtual bool Shutdown();

    virtual std::vector<PeerId> GetNeighborPeerIds();

//        virtual void StopOutgoingConnectionUpdate();

 private:
    PeerManager _peerManager;
    AddrManager _addrManager;
    AdvertisementManager _adManager;

    /* handler functions for each asynchronous event */
    void SocketConnectHandler(std::shared_ptr<DataSocket> dataSocket);

    void SocketConnectFailedHandler(std::string failedDomain);

    void SocketCloseHandler(std::shared_ptr<DataSocket> closedSocket);

    void PeerNotifyHandler(PeerId incomingPeerId, std::shared_ptr<DataSocket> socket);

    void RecvMsgHandler(PeerId sourcePeerId, std::shared_ptr<Message> msg);

 private:
    // periodic ping-pong check
    ev::timer _ping_timer;

    void _pingtimerCallback(ev::timer &w, int revents) {
        // Ping-pong mechanism for liveness check
        auto peers = _peerManager.GetPeers();
        for (auto&[peerId, peerPtr] : peers) {
            if (!peerPtr->IsPongReceived()) {
                // ping-pong did not worked for this peer
                // so, disconnect this peer.
                DisconnectPeer(peerId);
            }
        }


        peers = _peerManager.GetPeers();
        std::cout << "periodic ping" << "\n";
        for (auto const &pair : peers) {
            auto peerPtr = pair.second;
            std::shared_ptr<Message> pingMsg = std::make_shared<Message>(_peerManager.GetMyPeerId(),
                                                                            peerPtr->GetPeerId(), "PING");
            std::cout << "send PING to " << peerPtr->GetPeerId().GetId() << "\n";
            SendMsgToPeer(peerPtr->GetPeerId(), pingMsg);

            // reset ping received flag
            peerPtr->SetPongReceived(false);
        }
    }

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
                    if (peer && peer->IsActive()) {
                        continue;
                    } else if (peer && peer->IsTryConnect()) {
                        continue;
                    } else if (peer) {
                        std::cout << "connect to existing peer " << addr->GetString() << "\n";
                        // TODO : when it is happened? and what is the correct implementation?
                        libBLEEP::M_Assert(0, "to be implemented");
                        break;
                    } else {
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
}  // namespace libBLEEP_BL

#endif  // BLEEPLIB_SRC_BL2_PEER_CONNECTIVITY_PEERCONNECTIVITYLAYER_API_H_
