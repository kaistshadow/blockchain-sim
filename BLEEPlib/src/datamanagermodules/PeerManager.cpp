#include "PeerManager.h"
#include "../utility/Assert.h"
#include "shadow_interface.h"

#include <algorithm>
#include <stdio.h>

using namespace libBLEEP;

void libBLEEP::PeerManager::InitMyPeerId(std::string id) {
    _myPeerId = std::make_shared<PeerId>(id);
}

std::shared_ptr<PeerId> libBLEEP::PeerManager::GetMyPeerId() {
    M_Assert(_myPeerId != nullptr, "my peerId is used without initialization");
    return _myPeerId;
}

std::shared_ptr<PeerId> libBLEEP::PeerManager::GetPeerIdBySocket(int socketfd) {
    auto it = std::find_if(_neighborPeers.begin(), _neighborPeers.end(),
                           [socketfd](const std::pair<PeerId, std::shared_ptr<PeerInfo> > & t) -> bool {
                               return (t.second->GetSocketFD() == socketfd ||
                                       t.second->GetSocketFDRemote() == socketfd);
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
                               return (t.second->GetSocketFD() == socketfd ||
                                       t.second->GetSocketFDRemote() == socketfd);
                          } );
    if (it == _neighborPeers.end()) {
        std::cout << "No peer exists" << "\n";
        return nullptr;
    }
    return it->second;
}


std::vector<PeerId> libBLEEP::PeerManager::GetNeighborPeerIds(PeerConnectMode mode){
    std::vector<PeerId> id;
    std::map <PeerId, std::shared_ptr<PeerInfo>, PeerIdCompare>::iterator i = _neighborPeers.begin();
    while (i != _neighborPeers.end()){
        switch (mode){
            case PeerConnectMode::ConnectMyself:
                if ((i->second)->GetSocketStatus() == SocketStatus::SocketConnected){
                    id.push_back(i->first);
                }
                break;
            case PeerConnectMode::ConnectByRemote:
                if ((i->second)->GetSocketStatusRemote() == SocketStatus::SocketConnected){
                    id.push_back(i->first);
                }
                break;
            default :
                id.push_back(i->first);
                break;
        }
        i++;
    }
    return id;
}

// std::shared_ptr<PeerInfo> libBLEEP::PeerManager::AppendNewNeighborPeer(PeerId peer) {
//     std::shared_ptr<PeerInfo> peerPtr = GetPeerInfo(peer);
//     if (peerPtr) {
//         std::cout << "Peer already exists for " << peer.GetId() << "\n";
//         return peerPtr;
//     }

//     // allocate new peer
//     // and append it into the map structure (_neighborPeers)
//     _neighborPeers[peer] = std::make_shared<PeerInfo>();
//     std::cout << "PeerManager appends NewNeighbor for " << peer.GetId() << "\n";
//     return _neighborPeers[peer];
// }

std::shared_ptr<PeerInfo> libBLEEP::PeerManager::AppendNeighborPeerConnectedByMyself(PeerId peer, int socketfd) {
    std::shared_ptr<PeerInfo> peerPtr = GetPeerInfo(peer);
    M_Assert(!(peerPtr && peerPtr->GetSocketStatus() == SocketStatus::SocketConnected),
             "Why the redundant socket is requested? It should be avoided.");

    if (peerPtr) {
        // update socket information
        peerPtr->SetSocketFD(socketfd);
        peerPtr->SetSocketStatus(SocketStatus::SocketConnected);
        std::cout << "Peer already exists. Update connected socket info for " << peer.GetId() << "\n";
        return peerPtr;
    }

    // allocate new peer
    // and append it into the map structure (_neighborPeers)
    peerPtr = std::make_shared<PeerInfo>();
    peerPtr->SetSocketFD(socketfd);
    peerPtr->SetSocketStatus(SocketStatus::SocketConnected);
    _neighborPeers[peer] = peerPtr;
    std::cout << "PeerManager appends ConnectedNeighbor for " << peer.GetId() << "\n";
    return peerPtr;
}

std::shared_ptr<PeerInfo> libBLEEP::PeerManager::AppendNeighborPeerConnectedByRemote(PeerId peer, int socketfd) {
    std::shared_ptr<PeerInfo> peerPtr = GetPeerInfo(peer);
    M_Assert(!(peerPtr && peerPtr->GetSocketStatusRemote() == SocketStatus::SocketConnected),
             "Why the redundant socket is requested from same peer? It should be avoided.");

    if (peerPtr) {
        // update socket information
        peerPtr->SetSocketFDRemote(socketfd);
        peerPtr->SetSocketStatusRemote(SocketStatus::SocketConnected);
        std::cout << "Peer already exists. Update (remotely requested) connected socket info for "
                  << peer.GetId() << "\n";
        return peerPtr;
    }

    // allocate new peer
    // and append it into the map structure (_neighborPeers)
    peerPtr = std::make_shared<PeerInfo>();
    peerPtr->SetSocketFDRemote(socketfd);
    peerPtr->SetSocketStatusRemote(SocketStatus::SocketConnected);
    _neighborPeers[peer] = peerPtr;
    std::cout << "PeerManager appends (remotely requested) ConnectedNeighbor for " << peer.GetId() << "\n";
    return peerPtr;
}
// void libBLEEP::PeerManager::UpdateNeighborSocketDisconnection(PeerId peer) {
//     auto it = _neighborPeers.find(peer);
//     M_Assert(it != _neighborPeers.end(), "Not existed peer is disconnected?!");

//     std::shared_ptr<PeerInfo> peerInfo = it->second;
//     peerInfo->SetSocketFD(-1);
//     peerInfo->SetSocketStatus(SocketStatus::SocketDisconnected);
//     std::cout << "PeerManager set disconnection for " << peer.GetId() << "\n";
// }

void libBLEEP::PeerManager::UpdateNeighborSocketDisconnection(int socketfd) {
    auto it = std::find_if(_neighborPeers.begin(), _neighborPeers.end(),
                           [socketfd](const std::pair<PeerId, std::shared_ptr<PeerInfo> > & t) -> bool {
                               return t.second->GetSocketFD() == socketfd;
                          } );
    if (it != _neighborPeers.end()) {
        std::shared_ptr<PeerInfo> peerInfo = it->second;
        peerInfo->SetSocketFD(-1);
        peerInfo->SetSocketStatus(SocketStatus::SocketDisconnected);
        std::cout << "PeerManager set disconnection for " << it->first.GetId() << "\n";
    }

    auto it_remote = std::find_if(_neighborPeers.begin(), _neighborPeers.end(),
                           [socketfd](const std::pair<PeerId, std::shared_ptr<PeerInfo> > & t) -> bool {
                               return t.second->GetSocketFDRemote() == socketfd;
                          } );
    if (it_remote != _neighborPeers.end()) {
        std::shared_ptr<PeerInfo> peerInfo = it_remote->second;
        peerInfo->SetSocketFD(-1);
        peerInfo->SetSocketStatusRemote(SocketStatus::SocketDisconnected);
        std::cout << "PeerManager set disconnection of the (remotely requested) socket for " << it_remote->first.GetId() << "\n";
    }

    M_Assert( it == _neighborPeers.end() || it_remote == _neighborPeers.end(),
              "There should not exists a duplicated socket fd.");

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

bool libBLEEP::PeerManager::HasEstablishedDataSocket(PeerId peerId) {
    std::shared_ptr<PeerInfo> peerPtr = GetPeerInfo(peerId);
    if (peerPtr && (peerPtr->GetSocketStatus() == SocketStatus::SocketConnected ||
                    peerPtr->GetSocketStatusRemote() == SocketStatus::SocketConnected) ) {
        return true;
    }
    else
        return false;
}

int libBLEEP::PeerManager::GetConnectedSocketFD(PeerId peerId) {
    std::shared_ptr<PeerInfo> peerPtr = GetPeerInfo(peerId);

    if (peerPtr && peerPtr->GetSocketStatus() == SocketStatus::SocketConnected)
        return peerPtr->GetSocketFD();
    else if (peerPtr && peerPtr->GetSocketStatusRemote() == SocketStatus::SocketConnected)
        return peerPtr->GetSocketFDRemote();
    else
        M_Assert(0, "Check whether there exists established data socket using HasEstablishedDataSocket API");
    return -1;

}
