#ifndef PEER_MANAGER_H
#define PEER_MANAGER_H

#include <map>

#include "Peer.h"
#include "../BL1_socket/Socket.h"

namespace libBLEEP_BL {
    class PeerManager {
    private:
        PeerId _myPeerId;
        std::map<PeerId, std::shared_ptr<Peer>, PeerIdCompare> _peers;

    public:
        PeerManager(std::string id) : _myPeerId(PeerId(id)) {}
        PeerId& GetMyPeerId() { return _myPeerId; }

        
        void AddPeer(std::shared_ptr<Peer> peer);
        std::shared_ptr<Peer> FindPeer(PeerId peer); 
        
        std::map<PeerId, std::shared_ptr<Peer>, PeerIdCompare>& GetPeers() { return _peers;}
    };

}

#endif
