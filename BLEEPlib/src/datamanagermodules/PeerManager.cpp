#include "PeerManager.h"
#include "../utility/Assert.h"
#include "shadow_interface.h"

#include <algorithm>
#include <stdio.h>

using namespace libBLEEP;


void libBLEEP::PeerManager::InitMyPeerId(std::string id) {
    _myPeerId = std::make_shared<PeerId>(id);
    char buf[256];
    sprintf(buf, "InitPeerId,%s", id.c_str());
    shadow_push_eventlog(buf);
}

std::shared_ptr<PeerId> libBLEEP::PeerManager::GetMyPeerId() {
    M_Assert(_myPeerId != nullptr, "my peerId is used without initialization");    
    return _myPeerId;
}

std::shared_ptr<PeerId> libBLEEP::PeerManager::GetPeerIdBySocket(int socketfd) {
    auto it = std::find_if(_neighborPeers.begin(), _neighborPeers.end(),
                           [socketfd](const std::pair<PeerId, std::shared_ptr<PeerInfo> > & t) -> bool {
                               return t.second->GetSocketFD() == socketfd;
                          } );
    if (it == _neighborPeers.end()) {
        std::cout << "No peer exists" << "\n";
        return nullptr;
    }
    return std::make_shared<PeerId>(it->first);
}

std::shared_ptr<PeerInfo> libBLEEP::PeerManager::GetPeerInfo(PeerId peer) {
    auto it = _neighborPeers.find(peer);
    if (it == _neighborPeers.end()) {
        std::cout << "No peer exists" << "\n";
        return nullptr;
    }
    return it->second;
}

std::shared_ptr<PeerInfo> libBLEEP::PeerManager::GetPeerInfoBySocket(int socketfd) {
    auto it = std::find_if(_neighborPeers.begin(), _neighborPeers.end(),
                           [socketfd](const std::pair<PeerId, std::shared_ptr<PeerInfo> > & t) -> bool {
                               return t.second->GetSocketFD() == socketfd;
                          } );
    if (it == _neighborPeers.end()) {
        std::cout << "No peer exists" << "\n";
        return nullptr;
    }
    return it->second;
}

std::shared_ptr<PeerInfo> libBLEEP::PeerManager::AppendNewNeighborPeer(PeerId peer) {
    std::shared_ptr<PeerInfo> peerPtr = GetPeerInfo(peer);
    if (peerPtr) {
        std::cout << "Peer already exists" << "\n";
        return peerPtr;
    }

    // allocate new peer 
    // and append it into the map structure (_neighborPeers)
    _neighborPeers[peer] = std::make_shared<PeerInfo>();
    return _neighborPeers[peer];
}

std::shared_ptr<PeerInfo> libBLEEP::PeerManager::AppendConnectedNeighborPeer(PeerId peer, int socketfd) {
    std::shared_ptr<PeerInfo> peerPtr = GetPeerInfo(peer);
    if (peerPtr) {
        std::cout << "Peer already exists" << "\n";
        
        // update socket information
        peerPtr->SetSocketFD(socketfd);
        peerPtr->SetSocketStatus(SocketStatus::SocketConnected);
        return peerPtr;
    }

    // char buf[256];
    // sprintf(buf, "ConnectPeer,%s,%s", _myPeerId->GetId().c_str(), peer.GetId().c_str());
    // shadow_push_eventlog(buf);

    // allocate new peer 
    // and append it into the map structure (_neighborPeers)
    peerPtr = std::make_shared<PeerInfo>();
    peerPtr->SetSocketFD(socketfd);
    peerPtr->SetSocketStatus(SocketStatus::SocketConnected);
    _neighborPeers[peer] = peerPtr;
    return peerPtr;
}

void libBLEEP::PeerManager::UpdateNeighborSocketDisconnection(PeerId peer) {
    auto it = _neighborPeers.find(peer);
    M_Assert(it != _neighborPeers.end(), "Not existed peer is disconnected?!");

    std::shared_ptr<PeerInfo> peerInfo = it->second;
    peerInfo->SetSocketFD(-1);
    peerInfo->SetSocketStatus(SocketStatus::SocketDisconnected);
}

void libBLEEP::PeerManager::UpdateNeighborSocketDisconnection(int socketfd) {
    auto it = std::find_if(_neighborPeers.begin(), _neighborPeers.end(),
                           [socketfd](const std::pair<PeerId, std::shared_ptr<PeerInfo> > & t) -> bool {
                               return t.second->GetSocketFD() == socketfd;
                          } );
    if (it == _neighborPeers.end()) {
        // this case is possible
        // when redundant connection is closed by remote peer 
        std::cout << "Not existed peer is disconnected!" << "\n";
        return;
    }

    std::shared_ptr<PeerInfo> peerInfo = it->second;
    peerInfo->SetSocketFD(-1);
    peerInfo->SetSocketStatus(SocketStatus::SocketDisconnected);
}


// void libBLEEP::PeerManager::UpdateNeighborSocketConnection(PeerId peer, int socketfd) {
//     PeerInfo& info = _neighborPeers[peer];
//     info._socketfd = socketfd;
//     info._socketStatus = SocketStatus::SocketConnected;
// }

// void libBLEEP::PeerManager::UpdateNeighborSocketDisconnection(PeerId peer) {
//     PeerInfo& info = _neighborPeers[peer];
//     info._socketfd = -1;
//     info._socketStatus = SocketStatus::SocketDisconnected;
// }