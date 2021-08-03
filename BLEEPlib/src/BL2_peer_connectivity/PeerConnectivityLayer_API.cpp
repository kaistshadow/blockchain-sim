// "Copyright [2021] <kaistshadow>"

//
// Created by ilios on 21. 1. 26..
//
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <utility>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "../BL1_socket/SocketLayer_API.h"
#include "PeerConnectivityLayer_API.h"
#include "utility/Assert.h"
#include "utility/Random.h"
#include <iostream>

#include "shadow_interface.h"

using namespace libBLEEP_BL;

BL_PeerConnectivityLayer_API *BL_PeerConnectivityLayer_API::_instance = 0;

BL_PeerConnectivityLayer_API *BL_PeerConnectivityLayer_API::Instance() {
    if (_instance == 0) {
        libBLEEP::M_Assert(0, "Instance must be initialized first.");
    }
    return _instance;
}
void BL_PeerConnectivityLayer_API::InitInstance(std::string id) {
    libBLEEP::M_Assert(_instance == 0, "Instance must be initialized once.");
    _instance = new BL_PeerConnectivityLayer_API(id);
}


BL_PeerConnectivityLayer_API::BL_PeerConnectivityLayer_API(std::string myPeerId)
        : _peerManager(myPeerId), _adManager(Address(myPeerId)) {
    _startOutgoingConnectionUpdateTimer(10);

    // Initiate timer for sending a ping message periodically.
    _ping_timer.set<BL_PeerConnectivityLayer_API, &BL_PeerConnectivityLayer_API::_pingtimerCallback>(this);
    _ping_timer.set(60, 60); // ping-pong for every minute (60 seconds)
    _ping_timer.start();

    // append shadow log
    char buf[256];
    sprintf(buf, "InitPeerId,%s", myPeerId.c_str());
    shadow_push_eventlog(buf);
}

void BL_PeerConnectivityLayer_API::SocketConnectHandler(std::shared_ptr<DataSocket> dataSocket) {
    // The data socket is established for the requested peer connection,
    // thus, as a next step, need to proceed a peer establishment process (i.e., version handshaking).
    auto peers = _peerManager.GetPeers();

    // First, find peer for given data socket
    auto it = std::find_if(peers.begin(), peers.end(),
                           [dataSocket](const std::pair<PeerId, std::shared_ptr<Peer> > &t) -> bool {
                               return (t.second->GetConnSocket() == dataSocket->GetFD());
                           });
    if (it == peers.end()) {
        libBLEEP::M_Assert(0, "no proper peer exists for connected outgoing socket");
    }

    std::shared_ptr<Peer> outgoingPeer = it->second;
    // clearTryConnect flag
    outgoingPeer->ClearTryConnect();

    // assign a given (valid) datasocket for the peer object
    outgoingPeer->SetDataSocket(dataSocket);

    // append shadow log for connection estabilishment
    // Currently, we replace bitcoin-like version handshaking with a single notifyPeerId Msg.
    // Since socket establishment is already 3-way handshaking,
    // and the peer should try to make connection for valid outgoing peer,
    // we can assume that two peers are valid if socket connection is successfully established.
    char buf[256];
    sprintf(buf, "ConnectPeer,%s,%s",
            _peerManager.GetMyPeerId().GetId().c_str(),
            outgoingPeer->GetPeerId().GetId().c_str());
    shadow_push_eventlog(buf);

    // proceed a version handshaking process
    // TODO : implementation of bitcoin-like full version handshaking protocol
    std::shared_ptr<Message> message = std::make_shared<Message>(_peerManager.GetMyPeerId(), outgoingPeer->GetPeerId(),
                                                                 "notifyPeerId");
    // send notifyPeerId message
    std::cout << "Send notifyPeerId MSG" << "\n";
    SendMsgToPeer(outgoingPeer->GetPeerId(), message);
    std::cout << "Sent notifyPeerId MSG" << "\n";
    // send GETADDR protocol message
    std::shared_ptr<Message> getAddrMsg = std::make_shared<Message>(_peerManager.GetMyPeerId(),
                                                                    outgoingPeer->GetPeerId(), "GETADDR");
    std::cout << "Send GETADDR MSG" << "\n";
    SendMsgToPeer(outgoingPeer->GetPeerId(), getAddrMsg);

    // Append a peer address information for new outgoing peer
    _addrManager.Add(Address(outgoingPeer->GetPeerId().GetId(), libBLEEP::GetGlobalClock()));

    // Reserver advertisement of my address for new outgoing peer
    _adManager.AdvertiseLocal(outgoingPeer->GetPeerId());
}

void BL_PeerConnectivityLayer_API::SocketConnectFailedHandler(std::string failedDomain) {
    // check whether there's already active peer (i.e., peer with valid data socket)
    std::shared_ptr<Peer> peer = _peerManager.FindPeer(PeerId(failedDomain));
    if (peer && peer->IsActive()) {
        libBLEEP::M_Assert(0, "SocketConnectFailed for valid peer?!");
    }
    if (peer && peer->IsTryConnect()) {
        // clearTryConnect flag
        peer->ClearTryConnect();
    }
}

void BL_PeerConnectivityLayer_API::SocketCloseHandler(std::shared_ptr<DataSocket> closedSocket) {
    // The data socket is closed.
    // thus, we need to disconnect corresponding peer if necessary
    auto peers = _peerManager.GetPeers();

    if (closedSocket == nullptr) {
        std::cout << "null" << "\n";
    } else {
        std::cout << "fd:" << closedSocket->GetFD() << "\n";
    }
    // First, find peer for given data socket
    auto it = std::find_if(peers.begin(), peers.end(),
                           [closedSocket](const std::pair<PeerId, std::shared_ptr<Peer> > &t) -> bool
                           {
                               if (t.second->GetDataSocket() &&
                                   (t.second->GetDataSocket()->GetFD() == closedSocket->GetFD()))
                                   return true;
                               else
                                   return false;
                           });
    if (it == peers.end()) {
        std::cout << "socket closed and the socket is not managed by any peers" << "\n";
        // This case is possible when the incoming socket duplicated peer.
        return;
    }

    std::shared_ptr<Peer> peer = it->second;
    PeerId id = it->first;
    std::cout << "aa" << "\n";
    if (peer->GetPeerType() == PeerType::IncomingPeer) {
        char buf[256];
        sprintf(buf, "DisconnectIncomingPeer,%s,%s",
                _peerManager.GetMyPeerId().GetId().c_str(),
                id.GetId().c_str());
        shadow_push_eventlog(buf);
    } else if (peer->GetPeerType() == PeerType::OutgoingPeer) {
        char buf[256];
        sprintf(buf, "DisconnectOutgoingPeer,%s,%s",
                _peerManager.GetMyPeerId().GetId().c_str(),
                id.GetId().c_str());
        shadow_push_eventlog(buf);
    }

    _peerManager.RemovePeer(peer);
}

void BL_PeerConnectivityLayer_API::RecvMsgHandler(PeerId sourcePeerId,
                                              std::shared_ptr<Message> msg) {
    char buf[256];
    sprintf(buf, "RecvMessage,%s,%s,%s,%s",
            sourcePeerId.GetId().c_str(),
            _peerManager.GetMyPeerId().GetId().c_str(),
            msg->GetType().c_str(),
            msg->GetMessageId().c_str());
    shadow_push_eventlog(buf);


    MessageType msgType = msg->GetType();
    if (msgType == "notifyPeerId") {
        // this is initial message, and already handled by separate event ('PeerRecvNotifyPeerId')
        // thus do nothing
    } else if (msgType == "GETADDR") {
        std::cout << "received GETADDR" << "\n";
        std::vector<Address> vaddr = _addrManager.GetAddresses();
        // send ADDR message
    } else if (msgType == "ADDR") {
        std::cout << "received ADDR" << "\n";
        std::shared_ptr<AddrAd> addrAd = std::static_pointer_cast<AddrAd>(msg->GetObject());

        std::string myAddr = _peerManager.GetMyPeerId().GetId();
        std::vector<Address>& vAddr = addrAd->GetVAddr();
        int size = vAddr.size();
        for (auto &addr : vAddr) {
            std::cout << "loop" << "\n";
            if (addr.GetString() != myAddr)
                _addrManager.Add(addr);

            std::cout << "after add" << "\n";
            // relay address
            if (size <= 10) {
                auto rng = *(libBLEEP::get_global_random_source().get_default_random_source());
                std::uniform_int_distribution<> dist(1,2); // number of relay
                int relayCount = dist(rng);
                std::map<PeerId, std::shared_ptr<Peer>, PeerIdCompare>& peers = _peerManager.GetPeers();
                // std::set<int> indexset;
                for (int i = 0; i < relayCount; i++) {
                    std::uniform_int_distribution<> distv(0, peers.size()-1);
                    // indexset.insert(distv(rng));
                    auto it = peers.begin();
                    int random = distv(rng);
                    std::advance(it, random);
                    std::cout << "retrieving relayedPeer" << "\n";
                    std::shared_ptr<Peer> relayedPeer = it->second;
                    if (relayedPeer->GetPeerId().GetId() != msg->GetSource().GetId()) {
                        std::cout << "Relay addr(" << addr.GetString() << ") to "
                                  << relayedPeer->GetPeerId().GetId() << "\n";

                        _adManager.PushAddr(relayedPeer->GetPeerId(), addr);
                    }
                }
            }
        }
    } else if (msgType == "PING") {
        // Reply a pong message when the peer receives ping message.
        std::cout << "received PING" << "\n";

        std::shared_ptr<Message> pongMsg = std::make_shared<Message>(_peerManager.GetMyPeerId(), sourcePeerId, "PONG");
        SendMsgToPeer(sourcePeerId, pongMsg);
    } else if (msgType == "PONG") {
        // update ping received flag
        auto peerPtr = _peerManager.FindPeer(sourcePeerId);
        peerPtr->SetPongReceived(true);
        std::cout << "receive PONG from " << sourcePeerId.GetId() << "\n";
    } else {
        // Other (protocol) messages are handled by Layer3 event (ProtocolRecvMsg)
        AsyncEvent event(AsyncEventEnum::ProtocolRecvMsg);
        event.GetData().SetProtocolMsg(msg);
        MainEventManager::Instance()->PushAsyncEvent(event);
    }
}

void BL_PeerConnectivityLayer_API::PeerNotifyHandler(PeerId incomingPeerId,
                                                 std::shared_ptr<DataSocket> incomingSocket) {
    // check whether there's already active peer (i.e., peer with valid data socket)
    std::shared_ptr<Peer> peer = _peerManager.FindPeer(incomingPeerId);
    if (peer && peer->IsActive()) {
        // libBLEEP::M_Assert(0, "PeerNotify is requested for duplicated peer!");
        // TODO : handle the multiple sockets for the duplicated peer.
        std::cout << "PeerNotify is requested for duplicated peer!" << "\n";
        return;
    }

    // Check whether the number of incoming connection exceeds the limit.
    if (_peerManager.GetIncomingPeerNum() >= MAX_INCOMINGPEER_NUM) {
        std::cout << "PeerNotify requested (i.e., new incoming connection is established), "
                     "but number of incoming connection exceeds its limit. " <<
                  "LIMIT:" << MAX_INCOMINGPEER_NUM << "current number of incoming connection:"
                  << _peerManager.GetIncomingPeerNum() << "\n";

        // Disconnect incoming socket, since we cannot make a peer connection due to limit
        BL_SocketLayer_API::Instance()->DisconnectSocket(incomingSocket->GetFD());
        return;
    }
    // TODO : Can a peer be an outgoing peer and an incoming peer at the same time?
    // Currently, we ignore duplicated incoming peer.
    // However, duplicated socket can exist.
    // Also, visualization of the connect is also not properly handled.
    // (Visualization of the edge reflects the socket establishment, not peer establishment)
    // They should be changed to more concise version.

    // Assign a new Peer using incomingSocket
    peer = std::make_shared<Peer>(incomingPeerId, PeerType::IncomingPeer, incomingSocket);

    // Add assigned peer to PeerManager
    _peerManager.AddPeer(peer);
}

void BL_PeerConnectivityLayer_API::SwitchAsyncEventHandler(AsyncEvent& event) {
    switch (event.GetType()) {
        case AsyncEventEnum::PeerSocketConnect: {
            std::shared_ptr<DataSocket> dataSocket = event.GetData().GetDataSocket();
            SocketConnectHandler(dataSocket);
            break;
        }
        case AsyncEventEnum::PeerSocketConnectFailed: {
            std::string failedDomain = event.GetData().GetFailedDomain();
            SocketConnectFailedHandler(failedDomain);
        }
        case AsyncEventEnum::PeerSocketClose: {
            std::cout << "PeerSocketClose" << "\n";
            std::shared_ptr<DataSocket> closedSocket = event.GetData().GetClosedSocket();
            SocketCloseHandler(closedSocket);
            break;
        }
        case AsyncEventEnum::PeerRecvMsg: {
            PeerId sourcePeerId = event.GetData().GetMsgSourceId();
            std::shared_ptr<Message> incomingMsg = event.GetData().GetMsg();
            RecvMsgHandler(sourcePeerId, incomingMsg);
            break;
        }
        case AsyncEventEnum::PeerRecvNotifyPeerId: {
            PeerId inPeerId = event.GetData().GetNeighborId();
            std::shared_ptr<DataSocket> incomingSocket = event.GetData().GetIncomingSocket();
            PeerNotifyHandler(inPeerId, incomingSocket);
            break;
        }
        default:
            break;
    }
}

bool BL_PeerConnectivityLayer_API::ConnectPeer(PeerId id) {
    // check whether there's active peer (i.e., peer with valid data socket)
    std::shared_ptr<Peer> peer = _peerManager.FindPeer(id);
    if (peer && peer->IsActive()) {
        std::cout << "ConnectPeer is requested for duplicated peer!" << "\n";
        return false;
    }
    // TODO (Disconnection Logic)
    //   : find invalid data socket for all existing peer and automatically disconnect.


    // Assign a new Peer while initializing a new connectSocket using SocketLayer API
    int conn_socket = BL_SocketLayer_API::Instance()->ConnectSocket(id.GetId());
    if (conn_socket == -1) {
        std::cout << "ConnectSocket is not working for given peer id." << "\n";
        return false;
    }

    peer = std::make_shared<Peer>(id, PeerType::OutgoingPeer, conn_socket);
    peer->SetTryConnect();

    // Add assigned peer to PeerManager
    _peerManager.AddPeer(peer);
    return true;
}

bool BL_PeerConnectivityLayer_API::DisconnectPeer(PeerId id) {
    // check whether there's active peer (i.e., peer with valid data socket)
    std::shared_ptr<Peer> peer = _peerManager.FindPeer(id);
    std::cout << "Disconnect Peer(" << id.GetId() << ")\n";
    if (!peer) {
        std::cout << "DisconnectPeer is requested for invalid peer!" << "\n";
        return false;
    }

    // check whether there's ongoing connection try for the peer
    if (peer->IsTryConnect()) {
        // remove connect socket
        BL_SocketLayer_API::Instance()->AbandonConnectSocket(peer->GetConnSocket());
    } else {
        libBLEEP::M_Assert(peer->GetDataSocket() != nullptr, "valid peer should have valid data socket");
        // remove data socket
        int socketFD = peer->GetDataSocket()->GetFD();
        std::shared_ptr<DataSocket> dSocket = peer->GetDataSocket();
        BL_SocketLayer_API::Instance()->DisconnectSocket(socketFD);
    }
//    if (!peer->IsActive()) {
//        std::cout << "DisconnectPeer is requested for inactive peer." << "\n";
//        std::cout << "Thus, just remove peer from peerManager." << "\n";
//        _peerManager.RemovePeer(peer);
//        return true;
//    }


    if (peer->GetPeerType() == PeerType::IncomingPeer) {
        char buf[256];
        sprintf(buf, "DisconnectIncomingPeer,%s,%s",
                _peerManager.GetMyPeerId().GetId().c_str(),
                id.GetId().c_str());
        shadow_push_eventlog(buf);
    } else if (peer->GetPeerType() == PeerType::OutgoingPeer) {
        char buf[256];
        sprintf(buf, "DisconnectOutgoingPeer,%s,%s",
                _peerManager.GetMyPeerId().GetId().c_str(),
                id.GetId().c_str());
        shadow_push_eventlog(buf);
    }

    _peerManager.RemovePeer(peer);

    return true;
}




bool BL_PeerConnectivityLayer_API::SendMsgToPeer(PeerId id, std::shared_ptr<Message> msg) {
    // check whether the given destination peer is valid
    std::shared_ptr<Peer> dest = _peerManager.FindPeer(id);
    if (!dest || !dest->IsActive())
        return false;

    msg->SetSource(_peerManager.GetMyPeerId());

    // serialize message obj into an std::string
    std::string serial_str;
    boost::iostreams::back_insert_device<std::string> inserter(serial_str);
    boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
    boost::archive::binary_oarchive oa(s);
    oa << msg;
    s.flush();

    // Before sending MSG, send the bleep magic value first.
    dest->GetDataSocket()->AppendToSendBuff(BLEEP_MAGIC, BLEEP_MAGIC_SIZE);

    // Before sending MSG, send the length of the MSG
    int message_len = serial_str.size();
    dest->GetDataSocket()->AppendToSendBuff((const char*)&message_len, sizeof(int));

    std::cout << "MSG length:" << message_len << "\n";

    const char *buf = serial_str.c_str();
    dest->GetDataSocket()->AppendToSendBuff(buf, message_len);

    // append shadow log
    char logbuf[256];
    sprintf(logbuf, "UnicastMessage,%s,%s,%s,%s",
            _peerManager.GetMyPeerId().GetId().c_str(),
            dest->GetPeerId().GetId().c_str(),
            msg->GetType().c_str(),
            msg->GetMessageId().c_str());
    shadow_push_eventlog(logbuf);

    return true;
}

bool BL_PeerConnectivityLayer_API::Shutdown() {
    auto peers = _peerManager.GetPeerIds();
    for ( auto peerId : peers) {
        DisconnectPeer(peerId);
        std::cout << "DisconnectPeer " << peerId.GetId() << "\n";
    }

    // append shadow log
    char buf[256];
    sprintf(buf, "ShutdownPeerId,%s", _peerManager.GetMyPeerId().GetId().c_str());
    shadow_push_eventlog(buf);

    std::cout << "Shutdown the peer" << "\n";
    exit(0);

    return true;
}

std::vector<PeerId> BL_PeerConnectivityLayer_API::GetNeighborPeerIds() {
    std::vector<PeerId> peerIds = _peerManager.GetPeerIds();
    std::vector<PeerId> results;
    for (auto peerId : peerIds) {
        std::shared_ptr<Peer> peer = _peerManager.FindPeer(peerId);
        if (peer && peer->IsActive())
            results.push_back(peerId);
    }
    return results;
}
