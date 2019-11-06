#include <iostream>

#include "PeerManager.h"
#include "../utility/Assert.h"

using namespace libBLEEP_BL;

void PeerManager::AddPeer(std::shared_ptr<Peer> peer) {
    std::cout << "Peer added for [" << peer->GetPeerId().GetId() << "]" <<"\n";
    _peers[peer->GetPeerId()] = peer;

    if (peer->GetPeerType() == PeerType::IncomingPeer) {
        _inPeerNum += 1;
    }
    else if (peer->GetPeerType() == PeerType::OutgoingPeer) {
        _outPeerNum += 1;
    }
}

std::shared_ptr<Peer> PeerManager::FindPeer(PeerId peer) {
    auto it = _peers.find(peer);
    if (it == _peers.end()) {
        std::cout << "No peer exists" << "\n";
        return nullptr;
    }
    return it->second;
}

void PeerManager::RemovePeer(std::shared_ptr<Peer> peer) {
    auto it = _peers.find(peer->GetPeerId());
    libBLEEP::M_Assert(it != _peers.end(), "Peer should exist in manager");

    std::cout << "datasocket count:" << it->second->GetDataSocket().use_count() << "\n";

    _peers.erase(it);

    if (peer->GetPeerType() == PeerType::IncomingPeer) {
        _inPeerNum -= 1;
        libBLEEP::M_Assert(_inPeerNum >= 0, "wrong peernum" );
    }
    else if (peer->GetPeerType() == PeerType::OutgoingPeer) {
        _outPeerNum -= 1;
        libBLEEP::M_Assert(_outPeerNum >= 0, "wrong peernum");
    }
}
