#include "RandomGossipNetworkModule.h"
#include "../utility/Random.h"

using namespace libBLEEP;

RandomGossipNetworkModule::RandomGossipNetworkModule(std::string myPeerId, MainEventManager* mainEventManager, int fanOutNum)
    : watcherManager(this, mainEventManager) {
    fanOut = fanOutNum;
    _mainEventManager = mainEventManager;

    char buf[256];
    sprintf(buf, "API,RandomGossipNetworkModuleConstructor,%s", myPeerId.c_str());
    shadow_push_eventlog(buf);

    int listenSocketFD = socketManager.CreateListenSocket();
    peerManager.InitMyPeerId(myPeerId);
    new ListenSocketWatcher(listenSocketFD, this, _mainEventManager);

    sprintf(buf, "InitPeerId,%s", myPeerId.c_str());
    shadow_push_eventlog(buf);

}

bool RandomGossipNetworkModule::InsertMessageSet(std::string messageId) {
    return messageSet.insert(messageId).second;
}

bool RandomGossipNetworkModule::ExistMessage(std::string messageId) {
    auto itr = messageSet.find(messageId);
    return (itr != messageSet.end());
}

bool RandomGossipNetworkModule::AsyncConnectPeer(PeerId id, double time) {
    if (peerManager.HasEstablishedDataSocket(id))
        return false;

    char buf[256];
    sprintf(buf, "API,AsyncConnectPeer,%s,%f", id.GetId().c_str(), time);
    shadow_push_eventlog(buf);

    if (time > 0) {
        new AsyncConnectTimer(id, time, this, _mainEventManager);
        return true;
    } else {
        int connecting_fd = socketManager.CreateNonblockConnectSocket(id.GetId());
        _asyncConnectPeerRequests.push_back(std::make_pair(id, connecting_fd));

        new ConnectSocketWatcher(connecting_fd, this, _mainEventManager);
        return true;
    }
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

bool RandomGossipNetworkModule::AsyncConnectPeers(std::vector<PeerId> &peerList, int peerNum, double time){
    PeerId myId = *peerManager.GetMyPeerId();
    auto neighborPeerIdSet = genNeighborPeerSet(myId, peerList);
    int i = 0;
    for(const Distance& dest : neighborPeerIdSet){
        if (i >= peerNum) break;
        if (RandomGossipNetworkModule::AsyncConnectPeer(dest.GetPeerId(), time) == false) return false;
        i++;
    }
    return true;
}

bool RandomGossipNetworkModule::UnicastMessage(PeerId dest, std::shared_ptr<Message> message) {
    if (!peerManager.HasEstablishedDataSocket(dest))
        return false;

    char buf[256];
    sprintf(buf, "API,UnicastMessage,%s,%s",
            dest.GetId().c_str(),
            message->GetType().c_str());
    shadow_push_eventlog(buf);

    int socketFD = peerManager.GetConnectedSocketFD(dest);
    std::shared_ptr<DataSocket_v2> dataSocket = socketManager.GetDataSocket(socketFD);

    dataSocket->AppendMessageToSendBuff(message);

    std::shared_ptr<DataSocketWatcher> dataSocketWatcher = watcherManager.GetDataSocketWatcher(socketFD);
    if (dataSocketWatcher)
        dataSocketWatcher->SetWritable();
    else
        M_Assert(0, "proper dataSocketWatcher not exist");

    sprintf(buf, "UnicastMessage,%s,%s,%s,%s",
            peerManager.GetMyPeerId()->GetId().c_str(),
            dest.GetId().c_str(),
            message->GetType().c_str(),
            message->GetMessageId().c_str());
    shadow_push_eventlog(buf);

    return true;
}

bool RandomGossipNetworkModule::DisconnectPeer(PeerId id) {
    std::shared_ptr<PeerInfo> peerInfo = peerManager.GetPeerInfo(id);
    if (peerInfo == nullptr ||
        (peerInfo->GetSocketStatus() != SocketStatus::SocketConnected &&
         peerInfo->GetSocketStatusRemote() != SocketStatus::SocketConnected)) {
        return false;
    }

    char buf[256];
    sprintf(buf, "API,DisconnectPeer,%s", id.GetId().c_str());
    shadow_push_eventlog(buf);

    if (peerInfo->GetSocketStatus() == SocketStatus::SocketConnected) {
        int socketFD = peerInfo->GetSocketFD();
        socketManager.RemoveDataSocket(socketFD);
        watcherManager.RemoveDataSocketWatcher(socketFD);

        peerManager.UpdateNeighborSocketDisconnection(socketFD);
    }

    if (peerInfo->GetSocketStatusRemote() == SocketStatus::SocketConnected) {
        int socketFD = peerInfo->GetSocketFDRemote();
        socketManager.RemoveDataSocket(socketFD);
        watcherManager.RemoveDataSocketWatcher(socketFD);
        peerManager.UpdateNeighborSocketDisconnection(socketFD);
    }

    sprintf(buf, "DisconnectPeer,%s,%s", peerManager.GetMyPeerId()->GetId().c_str(), id.GetId().c_str());
    shadow_push_eventlog(buf);

    return true;
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
    auto idxs = GenRandomNumSet(dests.size(), fanOut);
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

