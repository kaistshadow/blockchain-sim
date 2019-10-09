#include <iostream>

#include "PeerManager.h"

using namespace libBLEEP_BL;

void PeerManager::AddPeer(std::shared_ptr<Peer> peer) {
    std::cout << "Peer added for [" << peer->GetPeerId().GetId() << "]" <<"\n";
    _peers[peer->GetPeerId()] = peer;
}

std::shared_ptr<Peer> PeerManager::FindPeer(PeerId peer) {
    auto it = _peers.find(peer);
    if (it == _peers.end()) {
        std::cout << "No peer exists" << "\n";
        return nullptr;
    }
    return it->second;
}
