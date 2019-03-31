#include "MainEventManager.h"
#include "../utility/Assert.h"

// #include <sys/types.h>
#include <sys/socket.h>

using namespace libBLEEP;


MainEventManager::MainEventManager(std::string myPeerId) {
    _libev_loop = EV_DEFAULT;
    listenSocketManager.CreateListenSocket();
    peerManager.InitMyPeerId(myPeerId);
}

void MainEventManager::Wait() {

    while (true) {
        ev_run (_libev_loop, EVRUN_ONCE);

        _HandleShadowEvents();

        _HandleNetworkEvents();

        _HandleTimerEvents();

        if (_asyncEventTriggered)
            break;
    }
    _asyncEventTriggered = false; // reset
    return;
}

bool MainEventManager::AsyncConnectPeer(PeerId id, double time) {
    // check whether the socket connection already exists for given peerId
    std::shared_ptr<PeerInfo> peerInfo = peerManager.GetPeerInfo(id);
    if (peerInfo && peerInfo->GetSocketStatus() == SocketStatus::SocketConnected) {
        // already exists a valid socket connection
        return false;
    }
   
    if (time > 0) {
        new AsyncConnectTimer(id, time, this); //timer automatically started
        return true;
    } else {
        int connecting_fd = connectSocketManager.CreateNonblockConnectSocket(id.GetId());    
        _asyncConnectPeerRequests.push_back(std::make_pair(id, connecting_fd));
        return true;
    }     
}

void MainEventManager::AsyncGenerateRandomTransaction(double time) {
    new AsyncGenerateRandomTransactionTimer(time, this);
}

void MainEventManager::_HandleShadowEvents() {
    if (!shadowPipeManager.IsEventTriggered()) {
        printf("No event triggered for user(shadow)-io!\n");
        return;
    }

    // check shadow pipe event
    if (shadowPipeManager.IsEventTriggered()) {
        switch (shadowPipeManager.GetEventType()) {
        case ShadowPipeEventEnum::readEvent:
            {
                std::cout << "Currently there's no handler for shadow pipe event" << "\n";
                exit(-1);
                break;
            }
        case ShadowPipeEventEnum::none:
        case ShadowPipeEventEnum::writeEvent: 
            {
                std::cout << "Error! no valid shadow pipe event is triggered!" << "\n";
                exit(-1);
            }
        }
    }
}

void MainEventManager::_HandleNetworkEvents() {
    // Level 1 event handling : socket-IO event handling 
    // Based on level 1 events, if there's any 'Message Passing' events, handle them.
    // List of 'Message Passing' events : CompleteAsyncConnectPeer, RecvMessage

    // ********************************** socket-IO event handling ******************************** //
    // check whether any valid FD event is triggered
    if (!listenSocketManager.IsEventTriggered() && !connectSocketManager.IsEventTriggered() 
        && !dataSocketManager.IsEventTriggered()) {
        printf("No event triggered for socket-io!\n");
        return;
    }

    // check listenSocket event
    if (listenSocketManager.IsEventTriggered()) {
        switch (listenSocketManager.GetEventType()) {
        case SocketEventEnum::readEvent:
            {
                int fd = listenSocketManager.GetEventTriggeredFD();
                listenSocketManager.ClearEventTriggered(); // clear 
                std::shared_ptr<ListenSocket> listenSocket = listenSocketManager.GetListenSocket(fd);
    
                // Need to execute accept on listen socket and create data socket
                // Handle pending 'accept' until there's no remaining accept requests.
                while (1) {
                    int data_sfd = listenSocket->DoAccept();
                    if (data_sfd == -1)
                        break;
                    /* create new data socket */
                    dataSocketManager.CreateDataSocket(data_sfd);
                }
                break;
            }
        case SocketEventEnum::none:
        case SocketEventEnum::writeEvent:
        case SocketEventEnum::closeEvent:
            {
                std::cout << "invalid event is triggered. " << "\n";
                exit(-1);
            }
        }
    }

    // check connectSocket event
    if (connectSocketManager.IsEventTriggered()) {
        switch (connectSocketManager.GetEventType()) {
        case SocketEventEnum::readEvent:
        case SocketEventEnum::none:
        case SocketEventEnum::closeEvent:
            {
                std::cout << "invalid event is triggered. " << "\n";
                exit(-1);
            }
        case SocketEventEnum::writeEvent:
            {
                int fd = connectSocketManager.GetEventTriggeredFD();
                connectSocketManager.ClearEventTriggered(); // clear 

                // check status of socket
                int err = 0;
                socklen_t len = sizeof(err);
                if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0 ) {
                    perror("getsockopt"); // Solaris pending error?
                    exit(-1); 
                }
                if (err) {
                    if( ECONNREFUSED == err ) {
                        // connection is refused 
                        // M_Assert(0, "Connection is refused");
                    } else if( ETIMEDOUT == err ) {
                        // connection timeout
                        M_Assert(0, "Connection is timeout");
                    }

                    connectSocketManager.RemoveConnectSocket(fd);

                    // Get requested information (peerId)
                    auto it = std::find_if( _asyncConnectPeerRequests.begin(), _asyncConnectPeerRequests.end(),
                                            [fd](const std::pair<PeerId, int>& element) { 
                                                return element.second == fd; } );
                    M_Assert(it != _asyncConnectPeerRequests.end(), "connecting event must have matching request"); 
                    PeerId refusedPeerId = it->first;
                    _asyncConnectPeerRequests.erase(it);

                    // set asynchronous event 
                    _asyncEventTriggered = true;
                    _nextAsyncEvent = AsyncEventEnum::ErrorAsyncConnectPeer;
                    _dataManager.SetRefusedPeerId(refusedPeerId);
                    _dataManager.SetError(err);

                    return;
                }


                // Convert connectSocket to dataSocket
                connectSocketManager.RemoveConnectSocket(fd); /* remove connect socket structure */
                dataSocketManager.CreateDataSocket(fd); /* create data socket structure */


                // Get requested information (peerId)
                auto it = std::find_if( _asyncConnectPeerRequests.begin(), _asyncConnectPeerRequests.end(),
                                        [fd](const std::pair<PeerId, int>& element) { 
                                            return element.second == fd; } );
                M_Assert(it != _asyncConnectPeerRequests.end(), "connecting event must have matching request"); 
                PeerId connectedPeerId = it->first;
                int connectedSocketFD = it->second;
                _asyncConnectPeerRequests.erase(it);

                // Append connected peer information in peerManager
                peerManager.AppendConnectedNeighborPeer(connectedPeerId, connectedSocketFD);

                // set asynchronous event 
                _asyncEventTriggered = true;
                _nextAsyncEvent = AsyncEventEnum::CompleteAsyncConnectPeer;
                _dataManager.SetConnectedPeerId(connectedPeerId);
                _dataManager.SetConnectedSocketFD(connectedSocketFD);

                return;
            }
        }
    }

}

void MainEventManager::_HandleTimerEvents() {
    // nothing to do
}


