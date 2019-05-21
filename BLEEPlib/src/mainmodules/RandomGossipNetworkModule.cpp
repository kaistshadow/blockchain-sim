#include "RandomGossipNetworkModule.h"
#include "../utility/Random.h"

using namespace libBLEEP;

RandomGossipNetworkModule::RandomGossipNetworkModule(std::string myPeerId, MainEventManager* mainEventManager, int maxMulticastingNum)
    : BasicNetworkModule::BasicNetworkModule(myPeerId, mainEventManager){
        _maxMulticastingNum = maxMulticastingNum;
}

std::set<Distance, DistanceCmp> genNeighborPeerSet(PeerId myId, std::vector<PeerId> &neighborPeerIds){
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

bool RandomGossipNetworkModule::SendMulticastMsg(PeerId dest, std::shared_ptr<Message> message) {
    // check whether there exists a data socket for the destination peer
    if (!peerManager.HasEstablishedDataSocket(dest))
        return false;

    // get datasocket
    int socketFD = peerManager.GetConnectedSocketFD(dest);
    std::shared_ptr<DataSocket_v2> dataSocket = socketManager.GetDataSocket(socketFD);

    // append a message to socket
    dataSocket->AppendMessageToSendBuff(message);
    // set writable for data socket watcher
    std::shared_ptr<DataSocketWatcher> dataSocketWatcher = watcherManager.GetDataSocketWatcher(socketFD);
    if (dataSocketWatcher)
        dataSocketWatcher->SetWritable();
    else
        M_Assert(0, "proper dataSocketWatcher not exist");

    // append shadow log
    char buf[256];
    sprintf(buf, "MulticastingMessage,%s,%s,%s,%s",
            peerManager.GetMyPeerId()->GetId().c_str(),
            dest.GetId().c_str(),
            message->GetType().c_str(),
            message->GetMessageId().c_str());
    shadow_push_eventlog(buf);

    return true;
}
bool RandomGossipNetworkModule::MulticastMessage(std::shared_ptr<Message> message){
    char buf[256];
    PeerId myId = *peerManager.GetMyPeerId();
    std::vector<PeerId> dests = GetNeighborPeerIds();
    if (dests.size() == 0) return true;
    auto idxs = GenRandomNumSet(dests.size(), _maxMulticastingNum);
    for(std::vector<PeerId>::size_type i = 0 ; i < dests.size(); i++){
        if (idxs.find(i) != idxs.end()){
            if (checkSourcePeer(message, dests[i])){
              if(SendMulticastMsg(dests[i], message) == false)
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
    PeerId myId = *peerManager.GetMyPeerId();
    auto neighborPeerIdSet = genNeighborPeerSet(myId, peerlist);
    int i = 0;
    for(const Distance& dest : neighborPeerIdSet){
        if (i >= peerNum) break;
        if (AsyncConnectPeer(dest.GetPeerId(), time) == false) return false;
        i++;
    }
    return true;
}
