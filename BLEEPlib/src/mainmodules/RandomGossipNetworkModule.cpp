#include "RandomGossipNetworkModule.h"

using namespace libBLEEP;

RandomGossipNetworkModule::RandomGossipNetworkModule(std::string myPeerId, MainEventManager* mainEventManager)
    : BasicNetworkModule::BasicNetworkModule(myPeerId, mainEventManager){
}

std::set<Distance, DistanceCmp> RandomGossipNetworkModule::GenNeighborPeerSet(std::vector<PeerId> &neighborPeerIds){
    PeerId myId = *peerManager.GetMyPeerId();
    std::set<Distance, DistanceCmp> neighborPeerIdSet;
    UINT256_t myHashId = myId.GetIdHash();
    for(auto peer : neighborPeerIds){
        UINT256_t peerHashId = peer.GetIdHash();
        UINT256_t distance = myHashId ^ peerHashId;
        neighborPeerIdSet.insert(Distance(distance, peer));
    }
    return neighborPeerIdSet;
}

bool checkSourcePeer(std::shared_ptr<Message> msg, PeerId peerId){
    return (msg->GetSource().GetId() != peerId.GetId());
}

bool RandomGossipNetworkModule::MulticastMessage(std::vector<PeerId> dests, std::shared_ptr<Message> message, std::set<int> idxs){
    char buf[256];
    PeerId myId = *peerManager.GetMyPeerId();
    for(std::vector<PeerId>::size_type i = 0 ; i < dests.size(); i++){
        if (idxs.find(i) != idxs.end()){
            if (checkSourcePeer(message, dests[i])){
              if(UnicastMessage(dests[i], message) == false)
                  return false;
              sprintf(buf, "SendMessage from %s", myId.GetId().c_str());
              shadow_push_eventlog(buf);
            }
        }
    }
    return true;
}

std::vector<PeerId> RandomGossipNetworkModule::GetNeighborPeerIds(PeerConnectMode mode){
    return peerManager.GetNeighborPeerIds(mode);
}

PeerId RandomGossipNetworkModule::GetMyPeerId(){
    return *peerManager.GetMyPeerId();
}

bool RandomGossipNetworkModule::AsyncConnectPeers(std::vector<PeerId> &peerlist, int peerNum, int time){
    auto neighborPeerIdSet = GenNeighborPeerSet(peerlist);
    int i = 0;
    for(const Distance& dest : neighborPeerIdSet){
        if (i >= peerNum) break;
        if (AsyncConnectPeer(dest.GetPeerId(), time) == false) return false;
        i++;
    }
    return true;
}
