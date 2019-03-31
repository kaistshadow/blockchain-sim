#include "MainEventManager.h"
#include "../datamodules/Message.h"
#include "../utility/Assert.h"
#include "shadow_interface.h"

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

bool MainEventManager::UnicastMessage(PeerId dest, std::shared_ptr<Message> message) {
    // check whether there exists a data socket for the destination peer
    std::shared_ptr<PeerInfo> peerInfo = peerManager.GetPeerInfo(dest);
    if (!peerInfo || peerInfo->GetSocketStatus() != SocketStatus::SocketConnected)
        return false;
    
    // get datasocket
    int socketFD = peerInfo->GetSocketFD();
    std::shared_ptr<DataSocket> dataSocket = dataSocketManager.GetDataSocket(socketFD);
    
    // append a message to socket
    dataSocket->AppendMessageToSendBuff(message);

    // append shadow log
    char buf[256];
    sprintf(buf, "UnicastMessage,%s,%s,%s", 
            message->GetSource().GetId().c_str(), 
            message->GetDest().GetId().c_str(),
            message->GetType().c_str());
    shadow_push_eventlog(buf);

    return true;
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

    // check dataSocket event
    if (dataSocketManager.IsEventTriggered()) {
        switch (dataSocketManager.GetEventType()) {
        case SocketEventEnum::readEvent:
            {
                int fd = dataSocketManager.GetEventTriggeredFD();
                dataSocketManager.ClearEventTriggered(); // clear 

                std::shared_ptr<DataSocket> dataSocket = dataSocketManager.GetDataSocket(fd);

                // overall process of receiving data
                // retrieve raw stream data from socket and append them into recvBuffer
                // retrieve Message from recvBuffer if possible
                // return Message as asynchronous event

                std::shared_ptr<Message> message = dataSocket->DoRecv();
                if (message) {  
                    // set asynchronous event
                    _asyncEventTriggered = true;
                    _nextAsyncEvent = AsyncEventEnum::RecvMessage;
                    _dataManager.SetReceivedMsg(message);

                    // append shadow log
                    char buf[256];
                    sprintf(buf, "RecvMessage,%s,%s,%s", 
                            message->GetSource().GetId().c_str(), 
                            message->GetDest().GetId().c_str(),
                            message->GetType().c_str());
                    shadow_push_eventlog(buf);

                } else {
                    // check whether the close event is triggered during DoRecv
                    if (dataSocketManager.IsEventTriggered()) {
                        switch (dataSocketManager.GetEventType()) {
                        case SocketEventEnum::none:
                        case SocketEventEnum::readEvent:
                        case SocketEventEnum::writeEvent:
                            {
                                std::cout << "invalid event is triggered. " << "\n";
                                exit(-1);
                            }
                        case SocketEventEnum::closeEvent:
                            {
                                int socketFD = dataSocketManager.GetEventTriggeredFD();
                                dataSocketManager.ClearEventTriggered(); // clear 
                                dataSocketManager.RemoveDataSocket(socketFD);

                                // update peermanager for closed socket
                                peerManager.UpdateNeighborSocketDisconnection(socketFD);
                            }
                        }
                    }
                    
                }
                return;
            }
        case SocketEventEnum::writeEvent:
            {
                int fd = dataSocketManager.GetEventTriggeredFD();
                dataSocketManager.ClearEventTriggered(); // clear 

                std::shared_ptr<DataSocket> dataSocket = dataSocketManager.GetDataSocket(fd);

                // overall process of sending data
                // 1. serialize data as payload, to make generic Message containing the payload
                // 2. serialize Message, and push it to sendBuffer
                // 3. retrieve raw data from sendBuffer and send them into socket.
                
                // write data to dataSocket
                dataSocket->DoSend();
    
                return;
            }
        case SocketEventEnum::none:
            {
                std::cout << "invalid event is triggered. " << "\n";
                exit(-1);
            }
        case SocketEventEnum::closeEvent:
            {
                std::cout << "invalid event is triggered. " << "\n";
                std::cout << "currently not support close socket. " << "\n";
                exit(-1);
            }
        }
    }

}

void MainEventManager::_HandleTimerEvents() {
    // nothing to do
}


