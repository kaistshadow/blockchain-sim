#include "Peer.h"

Peer *PeerList::GetPeerByDomain(std::string domain) {
    for (auto peer : peerList) {
        if (peer->GetHostname() == domain) 
            return peer;
    }
    return nullptr;
}

Peer *PeerList::GetPeerByIP(std::string ipaddr) {
    for (auto peer : peerList) {
        if (peer->GetIP() == ipaddr) 
            return peer;
    }
    return nullptr;
}

