#include "../datamanagermodules/SocketManager_v2.h"
#include "../datamanagermodules/PeerManager.h"

#include "MainEventManager.h"
#include "../datamodules/Message.h"
#include "../datamodules/Peer.h"
#include "../utility/Assert.h"

#include "shadow_interface.h"
#include <sys/socket.h>



namespace libBLEEP {


    class Distance {
        UINT256_t distance;
        PeerId peerId;

    public:
        Distance(UINT256_t distance, PeerId peerId)
            : distance(distance), peerId(peerId) {}
        PeerId GetPeerId() const { return peerId; }
        UINT256_t GetPeerDistance() const { return distance; }

        friend struct DistanceCmp;

    };

    struct DistanceCmp {
        bool operator()(const Distance& d1, const Distance& d2) const {
            return d1.distance > d2.distance;
        }
    };


    class RandomGossipNetworkModule {
    private:
        int fanOut;
        std::set<std::string> messageSet;
        class ListenSocketWatcher;
        class DataSocketWatcher;
        class ConnectSockerWatcher;

        class WatcherWrapper {
        protected:
            RandomGossipNetworkModule* _networkModule;
            MainEventManager* _mainEventModule;
        public:
            WatcherWrapper(RandomGossipNetworkModule* netModule, MainEventManager* eventModule)
                : _networkModule(netModule), _mainEventModule(eventModule){};

        };

        class WatcherManager {
        private:
            RandomGossipNetworkModule* _networkModule;
            MainEventManager* _mainEventModule;

            std::map<int, std::shared_ptr<DataSocketWatcher>> _dataSocketWatchers;
        public:
            WatcherManager(RandomGossipNetworkModule* netModule, MainEventManager* eventModule)
                : _networkModule(netModule), _mainEventModule(eventModule) {};
            void CreateDataSocketWatcher(int fd) {
            M_Assert(_dataSocketWatchers.find(fd) == _dataSocketWatchers.end(), "fd must not have duplicated watchers");
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
                if (it == _dataSocketWatchers.end())
                    _dataSocketWatchers.erase(it);
                }
        };

        class AsyncConnectTimer : public WatcherWrapper {
        private:
            ev::timer _timer;
            PeerId _id;
            void _timerCallback(ev::timer &w, int revents) {
                std::cout << "AsyncConnect timer callback executes!" << "\n";
                _networkModule->AsyncConnectPeer(_id);
                delete this;
            }
        public:
            AsyncConnectTimer(PeerId id, double time, RandomGossipNetworkModule* netModule, MainEventManager* eventModule)
                : WatcherWrapper (netModule, eventModule) {
                    _id = id;
                    _timer.set<AsyncConnectTimer, &AsyncConnectTimer::_timerCallback> (this);
                    _timer.set(time,0.);
                    _timer.start();
                    std::cout << "timer started!" << "\n";
            }
        };

        class ListenSocketWatcher : public WatcherWrapper {
        private:
            ev::io _watcher;
            void _listenSocketIOCallback(ev::io &w, int revents){
                std::cout << "listen socket IO callback called!" << "\n";

                if (revents & EV_READ){
                    int fd = w.fd;
                    std::shared_ptr<ListenSocket_v2> listenSocket = _networkModule->socketManager.GetListenSocket(fd);
                    while(1) {
                        int data_sfd = listenSocket->DoAccept();
                        if(data_sfd == -1) break;
                        _networkModule->socketManager.CreateDataSocket(data_sfd);
                        _networkModule->watcherManager.CreateDataSocketWatcher(data_sfd);
                    }
                }
            }
        public:
            ListenSocketWatcher(int fd, RandomGossipNetworkModule* netModule, MainEventManager* eventModule)
                : WatcherWrapper(netModule, eventModule) {
                    _watcher.set<ListenSocketWatcher, &ListenSocketWatcher::_listenSocketIOCallback> (this);
                    _watcher.start(fd, ev::READ);
            }
        };

        class DataSocketWatcher : public WatcherWrapper {
        private:
            int _fd;
            ev::io _watcher;

            void _dataSocketIOCallback(ev::io &w, int revents) {
                M_Assert(_fd == w.fd, "fd must be same");
                int fd = w.fd;
                std::cout << "data socket IO callback called!" << "\n";

                if (revents & EV_READ) {
                    std::shared_ptr<DataSocket_v2> dataSocket = _networkModule->socketManager.GetDataSocket(fd);
                    std::pair<bool, std::shared_ptr<Message>> recvResult = dataSocket->DoRecv();
                    if (!recvResult.first) {
                        std::shared_ptr<PeerId> disconnectedPeerId = _networkModule->peerManager.GetPeerIdBySocket(fd);
                        _networkModule->peerManager.UpdateNeighborSocketDisconnection(fd);
                        if (disconnectedPeerId != nullptr) {
                            AsyncEvent event(AsyncEventEnum::PeerDisconnected);
                            event.GetData().SetDisconnectedPeer(disconnectedPeerId);
                            _mainEventModule->PushAsyncEvent(event);
                        }

                        _networkModule->socketManager.RemoveDataSocket(fd);
                        _networkModule->watcherManager.RemoveDataSocketWatcher(fd);
                    } else {
                        std::shared_ptr<Message> message = recvResult.second;
                        if (message && message->GetType() == "notifyPeerId") {
                            _networkModule->peerManager.AppendNeighborPeerConnectedByRemote(message->GetSource().GetId(), fd);
                            std::shared_ptr<PeerId> newlyConnectedNeighborPeer = std::make_shared<PeerId>(message->GetSource());
                            AsyncEvent event(AsyncEventEnum::NewPeerConnected);
                            event.GetData().SetNewlyConnectedPeer(newlyConnectedNeighborPeer);
                            _mainEventModule->PushAsyncEvent(event);

                        } else if (message) {
                            AsyncEvent event(AsyncEventEnum::RecvMessage);
                            event.GetData().SetReceivedMsg(message);
                            _mainEventModule->PushAsyncEvent(event);

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

                            if (message->GetDest().GetId() == "DestAll") {
                                if(true == _networkModule->InsertMessageSet(message->GetMessageId())){
                                    sprintf(buf, " NewTx %s %s",
                                            _networkModule->peerManager.GetMyPeerId()->GetId().c_str(),
                                            message->GetMessageId().c_str());
                                    shadow_push_eventlog(buf);
                                    _networkModule->MulticastMessage(message);
                                }
                            }

                        }

                    }

                } else if (revents & EV_WRITE) {
                    std::shared_ptr<DataSocket_v2> dataSocket = _networkModule->socketManager.GetDataSocket(fd);
                    if (dataSocket->DoSend() == DoSendResultEnum::SendBuffEmptied)
                        UnsetWritable();
                }
            }
        public :
            DataSocketWatcher(int fd, RandomGossipNetworkModule* netModule, MainEventManager* eventModule)
                : WatcherWrapper(netModule, eventModule) {
                _watcher.set<DataSocketWatcher, &DataSocketWatcher::_dataSocketIOCallback> (this);
                _watcher.start(fd, ev::READ);

                _fd = fd;
                std::cout << "dataSockerWatcher created" << "\n";
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
            ev::io _watcher;

            void _connectSocketIOCallback (ev::io &w, int revents) {
                std::cout << "connect socket IO callback called!!!" << "\n";

                if (revents & EV_READ) {
                    std::cout << "Invalid event is triggered." << "\n";
                    exit(-1);
                } else if (revents & EV_WRITE) {
                    int fd = w.fd;
                    M_Assert(fd == _fd, "fd should be same");

                    int err = 0;
                    socklen_t len = sizeof(err);
                    if (getsockopt(_fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0) {
                        perror("getsockopt");
                        exit(-1);
                    }
                    auto it = std::find_if(_networkModule->_asyncConnectPeerRequests.begin(),
                                           _networkModule->_asyncConnectPeerRequests.end(),
                                           [fd](const std::pair<PeerId, int>& element) {
                                               return element.second == fd;
                                           });
                    M_Assert(it != _networkModule->_asyncConnectPeerRequests.end(), "connecting event must have matching request");
                    if (err) {
                        PeerId refusedPeerId = it->first;
                        _networkModule->_asyncConnectPeerRequests.erase(it);

                        AsyncEvent event(AsyncEventEnum::ErrorAsyncConnectPeer);
                        event.GetData().SetRefusedPeerId(refusedPeerId);
                        event.GetData().SetError(err);
                        if (ECONNREFUSED == err) {
                            event.GetData().SetErrorMsg("connection refused");
                        } else if (ETIMEDOUT == err) {
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

                    PeerId connectedPeerId = it->first;
                    int connectedSocketFD = it->second;
                    _networkModule->_asyncConnectPeerRequests.erase(it);

                    std::shared_ptr<PeerInfo> connPeer = _networkModule->peerManager.GetPeerInfo(connectedPeerId);
                    if (connPeer && connPeer->GetSocketStatus() == SocketStatus::SocketConnected) {
                        _networkModule->socketManager.RemoveDataSocket(fd);
                        _networkModule->watcherManager.RemoveDataSocketWatcher(fd);

                        AsyncEvent event(AsyncEventEnum::ErrorAsyncConnectPeer);
                        event.GetData().SetRefusedPeerId(connectedPeerId);
                        event.GetData().SetError(-1);
                        event.GetData().SetErrorMsg("redundant connection");
                        _mainEventModule->PushAsyncEvent(event);

                        _networkModule->socketManager.RemoveConnectSocket(fd);

                        close(fd);
                        delete this;
                        return;
                    }
                    _networkModule->socketManager.CreateDataSocket(fd);
                    _networkModule->watcherManager.CreateDataSocketWatcher(fd);

                    _networkModule->peerManager.AppendNeighborPeerConnectedByMyself(connectedPeerId, connectedSocketFD);

                    std::shared_ptr<Message> firstMsg = std::make_shared<Message>(*_networkModule->peerManager.GetMyPeerId(), connectedPeerId, "notifyPeerId", "");
                    std::shared_ptr<DataSocket_v2> dataSocket = _networkModule->socketManager.GetDataSocket(fd);
                    dataSocket->AppendMessageToSendBuff(firstMsg);

                    std::shared_ptr<DataSocketWatcher> dataSocketWatcher = _networkModule->watcherManager.GetDataSocketWatcher(fd);

                    if (dataSocketWatcher)
                        dataSocketWatcher->SetWritable();

                    AsyncEvent event(AsyncEventEnum::CompleteAsyncConnectPeer);
                    event.GetData().SetConnectedPeerId(connectedPeerId);
                    event.GetData().SetConnectedSocketFD(connectedSocketFD);
                    _mainEventModule->PushAsyncEvent(event);

                    char buf[256];
                    sprintf(buf, "ConnectPeer,%s,%s", _networkModule->peerManager.GetMyPeerId()->GetId().c_str(),connectedPeerId.GetId().c_str());
                    shadow_push_eventlog(buf);

                    _networkModule->socketManager.RemoveConnectSocket(fd);
                    delete this;
                    return;
                }
            }

        public:
            ConnectSocketWatcher(int fd, RandomGossipNetworkModule* netModule, MainEventManager* eventModule)
                : WatcherWrapper(netModule, eventModule) {
                    _watcher.set<ConnectSocketWatcher, &ConnectSocketWatcher::_connectSocketIOCallback> (this);
                    _watcher.start(fd, ev::WRITE);
                    _fd = fd;
                }


        };
    private:
        MainEventManager* _mainEventManager;
        SocketManager_v2 socketManager;

        WatcherManager watcherManager;

        PeerManager peerManager;

        std::list<std::pair<PeerId, int>> _asyncConnectPeerRequests;

        bool SendMulticastMsg(PeerId dest, std::shared_ptr<Message> message);

        std::vector<PeerId> GetNeighborPeerIds(PeerConnectMode mode = PeerConnectMode::none);

    public:
        RandomGossipNetworkModule(std::string myPeerId, MainEventManager* worker, int fanOutNum);

        PeerId GetMyPeerId();

        bool InsertMessageSet(std::string messageId);

        bool ExistMessage(std::string messageId);

        bool AsyncConnectPeer(PeerId id, double time=0);

        bool AsyncConnectPeers(std::vector<PeerId, std::allocator<PeerId>> &peerList, int peerNum, double time = 0);

        bool UnicastMessage(PeerId dest, std::shared_ptr<Message> message);

        bool MulticastMessage(std::shared_ptr<Message> message);

        bool DisconnectPeer(PeerId id);

    };

}

