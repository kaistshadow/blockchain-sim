#include "BasicNetworkModule.h"


using namespace libBLEEP;


BasicNetworkModule::BasicNetworkModule(std::string myPeerId, MainEventManager* mainEventManager)
    : watcherManager(this, mainEventManager) {
    _mainEventManager = mainEventManager;

    int listenSocketFD = socketManager.CreateListenSocket();
    peerManager.InitMyPeerId(myPeerId);
    new ListenSocketWatcher(listenSocketFD, this, _mainEventManager);
}

bool BasicNetworkModule::AsyncConnectPeer(PeerId id, double time) {
    // check whether the socket connection already exists for given peerId
    std::shared_ptr<PeerInfo> peerInfo = peerManager.GetPeerInfo(id);
    if (peerInfo && peerInfo->GetSocketStatus() == SocketStatus::SocketConnected) {
        // already exists a valid socket connection
        return false;
    }
   
    if (time > 0) {
        new AsyncConnectTimer(id, time, this, _mainEventManager); //timer automatically started
        return true;
    } else {
        int connecting_fd = socketManager.CreateNonblockConnectSocket(id.GetId());    
        _asyncConnectPeerRequests.push_back(std::make_pair(id, connecting_fd));

        // create event watcher (ConnectSocketWatcher) for the newly created ConnectSocket 
        new ConnectSocketWatcher(connecting_fd, this, _mainEventManager);
        return true;
    }     
}


bool BasicNetworkModule::UnicastMessage(PeerId dest, std::shared_ptr<Message> message) {
    // check whether there exists a data socket for the destination peer
    std::shared_ptr<PeerInfo> peerInfo = peerManager.GetPeerInfo(dest);
    if (!peerInfo || peerInfo->GetSocketStatus() != SocketStatus::SocketConnected)
        return false;
    
    // get datasocket
    int socketFD = peerInfo->GetSocketFD();
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
    sprintf(buf, "UnicastMessage,%s,%s,%s", 
            peerManager.GetMyPeerId()->GetId().c_str(),
            dest.GetId().c_str(),
            message->GetType().c_str());
    shadow_push_eventlog(buf);

    return true;
}

void BasicNetworkModule::AsyncGenerateRandomTransaction(double time) {
    new AsyncGenerateRandomTransactionTimer(time, this, _mainEventManager);
}

bool BasicNetworkModule::DisconnectPeer(PeerId id) {
    // check whether the socket connection exists for given peerId
    std::shared_ptr<PeerInfo> peerInfo = peerManager.GetPeerInfo(id);
    if (peerInfo == nullptr || peerInfo->GetSocketStatus() != SocketStatus::SocketConnected) {
        // there's no valid socket connection for given peerId
        return false;
    }

    int socketFD = peerInfo->GetSocketFD();

    // remove dataSocket 
    // it will automatically call destructor of DataSocket, thus automatically call close()
    socketManager.RemoveDataSocket(socketFD);
    // remove dataSocketWatcher
    // it will automatically call destructor of DataSocketWatcher (benefit of smart pointer)
    watcherManager.RemoveDataSocketWatcher(socketFD);

    // update peermanager for closed socket
    peerManager.UpdateNeighborSocketDisconnection(socketFD);

    // append shadow log for connection establishment
    char buf[256];
    sprintf(buf, "DisconnectPeer,%s,%s", peerManager.GetMyPeerId()->GetId().c_str(), id.GetId().c_str());
    shadow_push_eventlog(buf);

    return true;
}