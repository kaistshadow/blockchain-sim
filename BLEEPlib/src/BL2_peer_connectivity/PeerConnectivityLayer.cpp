#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "../utility/Assert.h"
#include "shadow_interface.h"

#include "PeerConnectivityLayer.h"

#include "../BL1_socket/SocketLayer_API.h"


using namespace libBLEEP_BL;

BL_PeerConnectivityLayer::BL_PeerConnectivityLayer(std::string myPeerId)
    : _peerManager(myPeerId) {
    // append shadow log
    char buf[256];
    sprintf(buf, "InitPeerId,%s", myPeerId.c_str());
    shadow_push_eventlog(buf);
}


void BL_PeerConnectivityLayer::SocketConnectHandler(std::shared_ptr<DataSocket> dataSocket) {
    // The data socket is established for the requested peer connection,
    // thus, as a next step, need to proceed a peer establishment process (i.e., version handshaking).
    auto peers = _peerManager.GetPeers();
    
    // First, find peer for given data socket
    auto it = std::find_if(peers.begin(), peers.end(),
                           [dataSocket](const std::pair<PeerId, std::shared_ptr<Peer> > &t) -> bool 
                           {
                               return (t.second->GetConnSocket() == dataSocket->GetFD()); 
                           });
    if (it == peers.end()) {
        libBLEEP::M_Assert(0, "no proper peer exists for connected outgoing socket");
    }

    // assign a given (valid) datasocket for the peer object
    std::shared_ptr<Peer> outgoingPeer = it->second;
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
    std::shared_ptr<Message> message = std::make_shared<Message>(_peerManager.GetMyPeerId(), outgoingPeer->GetPeerId(), "notifyPeerId");

    // send notifyPeerId message
    std::cout << "Send notifyPeerId MSG" << "\n";
    SendMsgToPeer(outgoingPeer->GetPeerId(), message);
}



void BL_PeerConnectivityLayer::RecvMsgHandler(PeerId sourcePeerId, 
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
        // this is initial message, and already handled by separate event ('PeerNotifyRecv')
        // thus do nothing
    }
}

void BL_PeerConnectivityLayer::PeerNotifyHandler(PeerId incomingPeerId, 
                                                 std::shared_ptr<DataSocket> incomingSocket) {
    // check whether there's already active peer (i.e., peer with valid data socket)    
    std::shared_ptr<Peer> peer = _peerManager.FindPeer(incomingPeerId);
    if (peer && peer->IsActive()) {
        // libBLEEP::M_Assert(0, "PeerNotify is requested for duplicated peer!");
        std::cout << "PeerNotify is requested for duplicated peer!" << "\n";
        return;
    }

    // TODO : Can a peer be an outgoing peer and an incoming peer at the same time?
    // currently, we ignore duplicated incoming peer.
    // However, duplicated socket are not handled. 
    // Visualization of the connect is also not properly handled. 
    // (as a result, visualization reflects the socket establishment, not peer establishment)
    // They should be changed to more concise version.

    // Assign a new Peer using incomingSocket
    peer = std::make_shared<Peer>(incomingPeerId, PeerType::IncomingPeer, incomingSocket);

    // Add assigned peer to PeerManager
    _peerManager.AddPeer(peer);
}

void BL_PeerConnectivityLayer::SwitchAsyncEventHandler(AsyncEvent& event) {
    switch (event.GetType()) {
    case AsyncEventEnum::PeerSocketConnect:
        {
            std::shared_ptr<DataSocket> dataSocket = event.GetData().GetDataSocket();
            SocketConnectHandler(dataSocket);
            break;
        }
    case AsyncEventEnum::PeerRecvMsg:
        {
            PeerId sourcePeerId = event.GetData().GetMsgSourceId();
            std::shared_ptr<Message> incomingMsg = event.GetData().GetMsg();
            RecvMsgHandler(sourcePeerId, incomingMsg);
            break;
        }
    case AsyncEventEnum::PeerNotifyRecv:
        {
            PeerId inPeerId = event.GetData().GetNeighborId();
            std::shared_ptr<DataSocket> incomingSocket = event.GetData().GetIncomingSocket();
            PeerNotifyHandler(inPeerId, incomingSocket);
            break;
        }
    default:
        break;        
    }
}

bool BL_PeerConnectivityLayer::ConnectPeer(PeerId id) {
    // check whether there's active peer (i.e., peer with valid data socket)    
    std::shared_ptr<Peer> peer = _peerManager.FindPeer(id);
    if (peer && peer->IsActive()) {
        std::cout << "ConnectPeer is requested for duplicated peer!" << "\n";
        return false;
    }
    // TODO (Disconnection Logic) 
    //   : find invalid data socket for all existing peer and automatically disconnect.


    // Assign a new Peer while initializing a new connectSocket using SocketLayer API
    int conn_socket = g_SocketLayer_API->ConnectSocket(id.GetId());
    peer = std::make_shared<Peer>(id, PeerType::OutgoingPeer, conn_socket);

    // Add assigned peer to PeerManager
    _peerManager.AddPeer(peer);


    return true;
}

bool BL_PeerConnectivityLayer::DisconnectPeer(PeerId id) {
    return false;
}

bool BL_PeerConnectivityLayer::SendMsgToPeer(PeerId id, std::shared_ptr<Message> msg) {
    // check whether the given destination peer is valid
    std::shared_ptr<Peer> dest = _peerManager.FindPeer(id);
    if (!dest || !dest->IsActive())
        return false;

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
