#ifndef BASIC_NETWORK_MODULE_H
#define BASIC_NETWORK_MODULE_H

#include "../datamanagermodules/SocketManager_v2.h"
#include "../datamanagermodules/PeerManager.h"

#include "MainEventManager.h"
#include "../datamodules/Message.h"
#include "../datamodules/Peer.h"
#include "../utility/Assert.h"

#include "shadow_interface.h"
#include <sys/socket.h>

namespace libBLEEP {

    class BasicNetworkModule {
    private:
        class ListenSocketWatcher;
        class DataSocketWatcher;
        class ConnectSocketWatcher;

        class WatcherWrapper {
        protected:
            BasicNetworkModule* _networkModule;
            MainEventManager* _mainEventModule;
        public:
            WatcherWrapper(BasicNetworkModule* netModule, MainEventManager* eventModule)
                : _networkModule(netModule), _mainEventModule(eventModule) {};
        };
        class WatcherManager {
        private:
            BasicNetworkModule* _networkModule;
            MainEventManager* _mainEventModule;

        public:
            /* event watcher */
            std::map<int, std::shared_ptr<DataSocketWatcher> > _dataSocketWatchers;

            WatcherManager(BasicNetworkModule* netModule, MainEventManager* eventModule)
                : _networkModule(netModule), _mainEventModule(eventModule) {};
            void CreateDataSocketWatcher(int fd) {
                M_Assert(_dataSocketWatchers.find(fd) == _dataSocketWatchers.end(), "fd must not have duplicated watchers");
                // allocate new DataSocketWatcher
                // and append it into the map structure (_dataSocketWatchers)
                _dataSocketWatchers[fd] = std::make_shared<DataSocketWatcher>(fd, _networkModule, _mainEventModule);
            }

            std::shared_ptr<DataSocketWatcher> GetDataSocketWatcher(int fd) {
                auto it = _dataSocketWatchers.find(fd);
                if (it == _dataSocketWatchers.end()) {
                    std::cout << "No valid dataSocketWatcher exists" << "\n";
                    return nullptr;
                }
                return it->second;
            }

            void RemoveDataSocketWatcher(int fd) {
                auto it = _dataSocketWatchers.find(fd);
                if (it != _dataSocketWatchers.end())
                    _dataSocketWatchers.erase(it);
            }

        };

        class AsyncConnectTimer : public WatcherWrapper {
        private:
            ev::timer _timer; // destructor automatically stops the watcher
            PeerId _id;
            void _timerCallback(ev::timer &w, int revents) {
                std::cout << "AsyncConnect timer callback executes!" << "\n";
                _networkModule->AsyncConnectPeer(_id);
                delete this;
            }
        public:
            AsyncConnectTimer(PeerId id, double time, BasicNetworkModule* netModule, MainEventManager* eventModule)
                : WatcherWrapper (netModule, eventModule) {
                _id = id;
                _timer.set<AsyncConnectTimer, &AsyncConnectTimer::_timerCallback> (this);
                _timer.set(time, 0.);
                _timer.start();
                std::cout << "timer started!" << "\n";
            }
        };

        class ListenSocketWatcher : public WatcherWrapper {
        private:
            /* event watcher */
            ev::io _watcher;

            /* event io callback */
            void _listenSocketIOCallback (ev::io &w, int revents) {
                std::cout << "listen socket IO callback called!" << "\n";

                if (revents & EV_READ) {
                    int fd = w.fd;

                    std::shared_ptr<ListenSocket_v2> listenSocket = _networkModule->socketManager.GetListenSocket(fd);

                    // Need to execute accept on listen socket and create data socket
                    // Handle pending 'accept' until there's no remaining accept requests.
                    while (1) {
                        int data_sfd = listenSocket->DoAccept();
                        if (data_sfd == -1)
                            break;
                        /* create new data socket */
                        _networkModule->socketManager.CreateDataSocket(data_sfd);
                        _networkModule->watcherManager.CreateDataSocketWatcher(data_sfd);
                    }
                }
            }
        public:
            ListenSocketWatcher(int fd, BasicNetworkModule* netModule, MainEventManager* eventModule)
                : WatcherWrapper(netModule, eventModule) {
                _watcher.set<ListenSocketWatcher, &ListenSocketWatcher::_listenSocketIOCallback> (this);
                _watcher.start(fd, ev::READ);
            }
        };

        class DataSocketWatcher : public WatcherWrapper {
        private:
            /* socket fd */
            int _fd;

            /* event watcher */
            ev::io _watcher;

            /* event io callback */
            void _dataSocketIOCallback (ev::io &w, int revents) {
                M_Assert(_fd == w.fd, "fd must be same");
                int fd = w.fd;
                std::cout << "data socket IO callback called!" << "\n";

                if (revents & EV_READ) {
                    std::shared_ptr<DataSocket_v2> dataSocket = _networkModule->socketManager.GetDataSocket(fd);

                    // overall process of receiving data
                    // retrieve raw stream data from socket and append them into recvBuffer
                    // retrieve Message from recvBuffer if possible
                    // return Message as asynchronous event

                    std::pair< bool, std::shared_ptr<Message> > recvResult = dataSocket->DoRecv();
                    if (!recvResult.first) {
                        // socket closed by remote while doRecv

                        std::shared_ptr<PeerId> disconnectedPeerId = _networkModule->peerManager.GetPeerIdBySocket(fd);
                        // update peermanager for closed socket
                        _networkModule->peerManager.UpdateNeighborSocketDisconnection(fd);

                        if (disconnectedPeerId != nullptr) {
                            // set asynchronous event
                            AsyncEvent event(AsyncEventEnum::PeerDisconnected);
                            event.GetData().SetDisconnectedPeer(disconnectedPeerId);
                            _mainEventModule->PushAsyncEvent(event);
                        }
                        else {
                            // this case is possible
                            // when redundant connection is closed by remote peer
                        }

                        _networkModule->socketManager.RemoveDataSocket(fd);
                        _networkModule->watcherManager.RemoveDataSocketWatcher(fd);
                        // automatically destroyed?
                    }
                    else {
                        // check whether any message is received
                        std::shared_ptr<Message> message = recvResult.second;
                        if (message && message->GetType() == "notifyPeerId") {
                            /** received id notify message (First message after socket establishment)    **/
                            /** This message is intended to be not visible to user of BasicNetworkModule.**/
                            /** Thus, we don't set recvMessage asynchronous event.                       **/
                            /** Instead, we only set newPeerConnected event.                             **/

                            // append a peer information
                            _networkModule->peerManager.AppendNeighborPeerConnectedByRemote(message->GetSource().GetId(), fd);

                            std::shared_ptr<PeerId> newlyConnectedNeighborPeer = std::make_shared<PeerId>(message->GetSource());
                            AsyncEvent event(AsyncEventEnum::NewPeerConnected);
                            event.GetData().SetNewlyConnectedPeer(newlyConnectedNeighborPeer);
                            _mainEventModule->PushAsyncEvent(event);
                        }
                        else if (message) {
                            // set asynchronous event
                            AsyncEvent event(AsyncEventEnum::RecvMessage);
                            event.GetData().SetReceivedMsg(message);
                            _mainEventModule->PushAsyncEvent(event);

                            // append shadow log
                            std::shared_ptr<PeerId> neighborPeerId = _networkModule->peerManager.GetPeerIdBySocket(fd);
                            std::cout << "message source:" << message->GetSource().GetId() << "\n";
                            M_Assert(neighborPeerId != nullptr, "no neighbor peer exists for given socket");

                            char buf[256];
                            sprintf(buf, "RecvMessage,%s,%s,%s,%s",
                                    neighborPeerId->GetId().c_str(),
                                    _networkModule->peerManager.GetMyPeerId()->GetId().c_str(),
                                    message->GetType().c_str(),
                                    message->GetMessageId().c_str());
                            shadow_push_eventlog(buf);

                        }
                    }

                } else if (revents & EV_WRITE) {
                    std::shared_ptr<DataSocket_v2> dataSocket = _networkModule->socketManager.GetDataSocket(fd);

                    // overall process of sending data
                    // 1. serialize data as payload, to make generic Message containing the payload
                    // 2. serialize Message, and push it to sendBuffer
                    // 3. retrieve raw data from sendBuffer and send them into socket.

                    // write data to dataSocket
                    if (dataSocket->DoSend() == DoSendResultEnum::SendBuffEmptied)
                        UnsetWritable();
                }
            }
        public:
            DataSocketWatcher(int fd, BasicNetworkModule* netModule, MainEventManager* eventModule)
                : WatcherWrapper(netModule, eventModule) {
                _watcher.set<DataSocketWatcher, &DataSocketWatcher::_dataSocketIOCallback> (this);
                _watcher.start(fd, ev::READ); // ev::WRITE will be set only when there exists any pending send request.
                _fd = fd;
                std::cout << "dataSocketWatcher created" << "\n";
            }
            ~DataSocketWatcher() {
                std::cout << "destructor called for dataSocketWatcher" << "\n";
            }

            void SetWritable() {
                _watcher.set(_fd, ev::READ | ev::WRITE);
            }

            void UnsetWritable() {
                _watcher.set(_fd, ev::READ);
            }
        };

        class ConnectSocketWatcher : public WatcherWrapper {
        private:
            int _fd;

            /* event watcher */
            ev::io _watcher;

            /* event io callback */
            void _connectSocketIOCallback (ev::io &w, int revents) {
                std::cout << "connect socket IO callback called!!!" << "\n";

                if (revents & EV_READ) {
                    std::cout << "invalid event is triggered. " << "\n";
                    exit(-1);
                } else if (revents & EV_WRITE) {
                    int fd = w.fd;
                    M_Assert(fd == _fd, "fd should be same");

                    // check status of socket
                    int err = 0;
                    socklen_t len = sizeof(err);
                    if (getsockopt(_fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0 ) {
                        perror("getsockopt"); // Solaris pending error?
                        exit(-1);
                    }
                    if (err) {
                        // Get requested information (peerId)
                        auto it = std::find_if( _networkModule->_asyncConnectPeerRequests.begin(), _networkModule->_asyncConnectPeerRequests.end(),
                                                [fd](const std::pair<PeerId, int>& element) {
                                                    return element.second == fd; } );
                        M_Assert(it != _networkModule->_asyncConnectPeerRequests.end(), "connecting event must have matching request");
                        PeerId refusedPeerId = it->first;
                        _networkModule->_asyncConnectPeerRequests.erase(it);

                        // set asynchronous event
                        AsyncEvent event(AsyncEventEnum::ErrorAsyncConnectPeer);
                        event.GetData().SetRefusedPeerId(refusedPeerId);
                        event.GetData().SetError(err);
                        if ( ECONNREFUSED == err ) {
                            event.GetData().SetErrorMsg("connection refused");
                        } else if ( ETIMEDOUT == err) {
                            event.GetData().SetErrorMsg("connection timeout");
                        } else {
                            M_Assert(0, "unknown connection error");
                        }
                        _mainEventModule->PushAsyncEvent(event);


                        _networkModule->socketManager.RemoveConnectSocket(fd);
                        close(fd);
                        delete this;
                        return;
                    }


                    // Get requested information (peerId)
                    auto it = std::find_if( _networkModule->_asyncConnectPeerRequests.begin(), _networkModule->_asyncConnectPeerRequests.end(),
                                            [fd](const std::pair<PeerId, int>& element) {
                                                return element.second == fd; } );
                    M_Assert(it != _networkModule->_asyncConnectPeerRequests.end(), "connecting event must have matching request");
                    PeerId connectedPeerId = it->first;
                    int connectedSocketFD = it->second;
                    _networkModule->_asyncConnectPeerRequests.erase(it);

                    std::shared_ptr<PeerInfo> connPeer = _networkModule->peerManager.GetPeerInfo(connectedPeerId);
                    if (connPeer && connPeer->GetSocketStatus() == SocketStatus::SocketConnected) {
                        // there already exists a connected neighbor peer.
                        // Thus remove newly generate dataSocket(since it's redundant)
                        _networkModule->socketManager.RemoveDataSocket(fd);
                        _networkModule->watcherManager.RemoveDataSocketWatcher(fd);

                        // return ErrorAsyncConnectPeer (redundant connection)
                        // set asynchronous event
                        AsyncEvent event(AsyncEventEnum::ErrorAsyncConnectPeer);
                        event.GetData().SetRefusedPeerId(connectedPeerId);
                        event.GetData().SetError(-1);
                        event.GetData().SetErrorMsg("redundant connection");
                        _mainEventModule->PushAsyncEvent(event);

                        // remove connect socket & socket watcher
                        _networkModule->socketManager.RemoveConnectSocket(fd); /* remove connect socket structure */
                        close(fd);
                        delete this;
                        return;
                    }

                    // Create new dataSocket for new connection
                    _networkModule->socketManager.CreateDataSocket(fd); /* create data socket structure */
                    _networkModule->watcherManager.CreateDataSocketWatcher(fd);

                    // Append connected peer information in peerManager
                    _networkModule->peerManager.AppendNeighborPeerConnectedByMyself(connectedPeerId, connectedSocketFD);

                    /** send id notify message (First message after socket establishment)   **/
                    /** Of course, we use socket for sending this message.                  **/
                    /** However, this message is not visible to user of MainEventManager.   **/
                    std::shared_ptr<Message> firstMsg = std::make_shared<Message>(*_networkModule->peerManager.GetMyPeerId(),
                                                                                  connectedPeerId,
                                                                                  "notifyPeerId", "");
                    std::shared_ptr<DataSocket_v2> dataSocket = _networkModule->socketManager.GetDataSocket(fd);
                    dataSocket->AppendMessageToSendBuff(firstMsg);
                    // set writable for data socket watcher
                    std::shared_ptr<DataSocketWatcher> dataSocketWatcher = _networkModule->watcherManager.GetDataSocketWatcher(fd);
                    if (dataSocketWatcher)
                        dataSocketWatcher->SetWritable();


                    // push asynchronous event
                    AsyncEvent event(AsyncEventEnum::CompleteAsyncConnectPeer);
                    event.GetData().SetConnectedPeerId(connectedPeerId);
                    event.GetData().SetConnectedSocketFD(connectedSocketFD);
                    _mainEventModule->PushAsyncEvent(event);

                    // append shadow log for connection establishment
                    char buf[256];
                    sprintf(buf, "ConnectPeer,%s,%s", _networkModule->peerManager.GetMyPeerId()->GetId().c_str(), connectedPeerId.GetId().c_str());
                    shadow_push_eventlog(buf);

                    // remove connect socket & socket watcher
                    _networkModule->socketManager.RemoveConnectSocket(fd); /* remove connect socket structure */
                    delete this;
                    return;
                }
            }
        public:
            ConnectSocketWatcher(int fd, BasicNetworkModule* netModule, MainEventManager* eventModule)
                : WatcherWrapper(netModule, eventModule) {
                _watcher.set<ConnectSocketWatcher, &ConnectSocketWatcher::_connectSocketIOCallback> (this);
                _watcher.start(fd, ev::WRITE);
                _fd = fd;
            }
        };

    private:
        MainEventManager* _mainEventManager;

    public:
        /* Constructor with proper peer id */
        BasicNetworkModule(std::string myPeerId, MainEventManager* worker);

        /*********************************************************/
        /* Public API designed for high-level event requests */
        /*********************************************************/

        /* asynchronous API that requests a connection for given peer */
        /* When the task is complete, 'complete event' will be triggered. */
        /* 'complete event' offers connected socket fd and requested PeerId */
        /* If the valid socket connection exists for given PeerId,
           the function immediately returns false, and 'complete event' will not be triggered */
        /* If the 'time' is given, the API tries connection
           after the given 'time' is passed. */
        bool AsyncConnectPeer(PeerId id, double time = 0);

        /* asynchronous API that sends a given message to proper peer(s) */
        /* If the valid socket connection does not exist for given PeerId,
           the function immediately returns false */
        /* There's no separate complete event for this API */
        bool UnicastMessage(PeerId dest, std::shared_ptr<Message> message);

        /* synchronous API that requests a disconnection for given peer */
        /* Since it's synchronous API,
           there's no separate complete event for this API */
        /* If the valid socket connection does not exist for given PeerId,
           the function immediately returns false */
        bool DisconnectPeer(PeerId id);

    private:
        /*********************************************************/
        /* Internal data structures
           for managing the asynchronous function call data.
           Current list of supported asynchronous function calls :
           1) AsyncConnectPeer (supported at 20190328)           */
        /*********************************************************/

        // for managing AsyncConnectPeer's requested data
        std::list<std::pair< PeerId, int > > _asyncConnectPeerRequests;


    private:
        WatcherManager watcherManager;

    protected:
        // Internal management module for managing peers
        // It maintains the table of the all known peers having valid socket connection
        PeerManager peerManager;

    private:
        /*********************************************************/
        /* Internal management modules for managing sockets */
        /*********************************************************/

        /*************** for socket-IO event handling ******************/
        SocketManager_v2 socketManager;

    };

}

#endif
