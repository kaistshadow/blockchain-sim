// "Copyright [2021] <kaistshadow>"

#ifndef BLEEPLIB_SRC_BL2_PEER_CONNECTIVITY_PEERMANAGER_H_
#define BLEEPLIB_SRC_BL2_PEER_CONNECTIVITY_PEERMANAGER_H_


#include <string>
#include <memory>
#include <vector>
#include <map>

#include "Peer.h"


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
    void RemovePeer(std::shared_ptr<Peer> peer);

    std::vector<PeerId> GetPeerIds() {
        std::vector<PeerId> ret;
        for (auto item : _peers) {
            ret.push_back(item.first);
        }
        return ret; }
    std::map<PeerId, std::shared_ptr<Peer>, PeerIdCompare>& GetPeers() { return _peers;}

    // management for current valid outgoing & incoming peers
 private:
    int _outPeerNum;
    int _inPeerNum;

 public:
    int GetOutgoingPeerNum() { return _outPeerNum; }
    int GetIncomingPeerNum() { return _inPeerNum; }
};

}  // namespace libBLEEP_BL

#endif  // BLEEPLIB_SRC_BL2_PEER_CONNECTIVITY_PEERMANAGER_H_
