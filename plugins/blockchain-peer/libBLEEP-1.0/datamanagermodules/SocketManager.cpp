#include "SocketManager.h"
#include "../utility/Assert.h"

#include <strings.h>
#include <unistd.h>
#include <algorithm>



using namespace libBLEEP;

int libBLEEP::ListenSocketManager::CreateListenSocket(int port) {
    // new ListenSocket
    std::shared_ptr<ListenSocket> new_socket = std::make_shared<ListenSocket>(port, this);
    _sockets[new_socket->GetFD()] = new_socket;

    // create event watcher for the ListenSocket    
    _listenSocketWatchers.emplace_back();     
    ev::io& socketIOW = _listenSocketWatchers.back();
    socketIOW.set<ListenSocketManager, &ListenSocketManager::_listenSocketIOCallback> (this);
    socketIOW.start(new_socket->GetFD(), ev::READ);

    return new_socket->GetFD();
}

std::shared_ptr<ListenSocket> libBLEEP::ListenSocketManager::GetListenSocket(int fd) {
    auto it = _sockets.find(fd);
    if (it == _sockets.end()) {
        std::cout << "No valid listenSocket exists" << "\n";
        return nullptr;
    }
    return it->second;
}

void libBLEEP::ListenSocketManager::_listenSocketIOCallback(ev::io &w, int revents) {
    std::cout << "listen socket IO callback called!" << "\n";
    
    _SetEventTriggered(true);
    _SetEventTriggeredFD(w.fd);
    _SetEventType(SocketEventEnum::readEvent);
}


int libBLEEP::ConnectSocketManager::CreateNonblockConnectSocket(std::string dest) {
    // new ConnectSocket
    std::shared_ptr<ConnectSocket> new_socket = std::make_shared<ConnectSocket>(dest, this);
    _sockets[new_socket->GetFD()] = new_socket;

    // create event watcher for the newly created ConnectSocket    
    int fd = new_socket->GetFD();
    M_Assert(_connectSocketWatchers.find(fd) == _connectSocketWatchers.end(), "fd must not be duplicated");

    // check https://stackoverflow.com/a/27553958 for emplacement of map structure
    ev::io& socketIOW = _connectSocketWatchers[fd]; // newly constructed io watcher is returned
    socketIOW.set<ConnectSocketManager, &ConnectSocketManager::_connectSocketIOCallback> (this);
    socketIOW.start(fd, ev::WRITE);

    return fd;
}

void libBLEEP::ConnectSocketManager::RemoveConnectSocket(int fd) {
    auto it = _sockets.find(fd);
    if ( it != _sockets.end())
        _sockets.erase(it);

    auto it_w = _connectSocketWatchers.find(fd);
    if ( it_w != _connectSocketWatchers.end())
        _connectSocketWatchers.erase(it_w);
}

void libBLEEP::ConnectSocketManager::_connectSocketIOCallback(ev::io &w, int revents) {
    M_Assert(revents & EV_WRITE, "write event should be triggered for connecting socket");

    std::cout << "connect socket IO callback called!" << "\n";
    
    _SetEventTriggered(true);
    _SetEventTriggeredFD(w.fd);
    _SetEventType(SocketEventEnum::writeEvent);
}

void libBLEEP::DataSocketManager::CreateDataSocket(int sfd) {
    // new DataSocket
    std::shared_ptr<DataSocket> new_socket = std::make_shared<DataSocket>(sfd, this);
    _sockets[new_socket->GetFD()] = new_socket;
    
    std::cout << "DataSocket created" << "\n";

    // create event watcher
    M_Assert(_dataSocketWatchers.find(sfd) == _dataSocketWatchers.end(), "fd must not have duplicated watchers");
    
    ev::io& socketIOW = _dataSocketWatchers[sfd]; // newly constructed io watcher is returned
    socketIOW.set<DataSocketManager, &DataSocketManager::_dataSocketIOCallback> (this);
    socketIOW.start(sfd, ev::READ); // ev::WRITE will be set only when there exists any pending send request.
}

void libBLEEP::DataSocketManager::SetWritable(int sfd) {
    M_Assert(_dataSocketWatchers.find(sfd) != _dataSocketWatchers.end(), "should contain proper watcher");

    ev::io& socketIOW = _dataSocketWatchers[sfd];
    socketIOW.set(sfd, ev::READ | ev::WRITE);
}

void libBLEEP::DataSocketManager::UnsetWritable(int sfd) {
    M_Assert(_dataSocketWatchers.find(sfd) != _dataSocketWatchers.end(), "should contain proper watcher");

    ev::io& socketIOW = _dataSocketWatchers[sfd];
    socketIOW.set(sfd, ev::READ);
}

void libBLEEP::DataSocketManager::RemoveDataSocket(int fd) {
    auto it = _sockets.find(fd);
    if ( it != _sockets.end())
        _sockets.erase(it);

    auto it_w = _dataSocketWatchers.find(fd);
    if ( it_w != _dataSocketWatchers.end())
        _dataSocketWatchers.erase(it_w);
}

std::shared_ptr<DataSocket> libBLEEP::DataSocketManager::GetDataSocket(int fd) {
    auto it = _sockets.find(fd);
    if (it == _sockets.end()) {
        std::cout << "No valid dataSocket exists" << "\n";
        return nullptr;
    }
    return it->second;
}

std::vector< std::shared_ptr<DataSocket> > libBLEEP::DataSocketManager::GetAllDataSockets() {
    std::vector< std::shared_ptr<DataSocket> > sockets;
    transform(_sockets.begin(), _sockets.end(), back_inserter(sockets), [](const std::map<int, std::shared_ptr<DataSocket> >::value_type& val){return val.second;} );

    return sockets;
}

void libBLEEP::DataSocketManager::_dataSocketIOCallback(ev::io &w, int revents) {
    std::cout << "data socket IO callback called!" << "\n";
    
    _SetEventTriggered(true);
    _SetEventTriggeredFD(w.fd);
    if (revents & EV_READ) // always read event is handled first. (TODO :: handle both events at the sametime)
        _SetEventType(SocketEventEnum::readEvent);
    else if (revents & EV_WRITE)
        _SetEventType(SocketEventEnum::writeEvent);
}
