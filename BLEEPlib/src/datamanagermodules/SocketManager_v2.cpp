#include "SocketManager_v2.h"
#include "../utility/Assert.h"
#include "../utility/Logger.h"

#include <strings.h>
#include <unistd.h>
#include <algorithm>


using namespace libBLEEP;

int libBLEEP::SocketManager_v2::CreateListenSocket(int port) {
    // new ListenSocket
    std::shared_ptr<ListenSocket_v2> new_socket = std::make_shared<ListenSocket_v2>(port);
    _listenSockets[new_socket->GetFD()] = new_socket;

    return new_socket->GetFD();
}

std::shared_ptr<ListenSocket_v2> libBLEEP::SocketManager_v2::GetListenSocket(int fd) {
    auto it = _listenSockets.find(fd);
    if (it == _listenSockets.end()) {
        gLog << "No valid listenSocket exists" << "\n";
        return nullptr;
    }
    return it->second;
}




void libBLEEP::SocketManager_v2::CreateDataSocket(int sfd) {
    // new DataSocket
    std::shared_ptr<DataSocket_v2> new_socket = std::make_shared<DataSocket_v2>(sfd);
    _dataSockets[new_socket->GetFD()] = new_socket;
    
    gLog << "DataSocket created" << "\n";
}

void libBLEEP::SocketManager_v2::RemoveDataSocket(int fd) {
    auto it = _dataSockets.find(fd);
    if ( it != _dataSockets.end())
        _dataSockets.erase(it);

}

std::shared_ptr<DataSocket_v2> libBLEEP::SocketManager_v2::GetDataSocket(int fd) {
    auto it = _dataSockets.find(fd);
    if (it == _dataSockets.end()) {
        gLog << "No valid dataSocket exists" << "\n";
        return nullptr;
    }
    return it->second;
}

std::vector< std::shared_ptr<DataSocket_v2> > libBLEEP::SocketManager_v2::GetAllDataSockets() {
    std::vector< std::shared_ptr<DataSocket_v2> > sockets;
    transform(_dataSockets.begin(), _dataSockets.end(), back_inserter(sockets), [](const std::map<int, std::shared_ptr<DataSocket_v2> >::value_type& val){return val.second;} );

    return sockets;
}

int libBLEEP::SocketManager_v2::CreateNonblockConnectSocket(std::string dest) {
    // new ConnectSocket
    std::shared_ptr<ConnectSocket_v2> new_socket = std::make_shared<ConnectSocket_v2>(dest);
    _connectSockets[new_socket->GetFD()] = new_socket;

    int fd = new_socket->GetFD();
    // M_Assert(_connectSocketWatchers.find(fd) == _connectSocketWatchers.end(), "fd must not be duplicated");

    // check https://stackoverflow.com/a/27553958 for emplacement of map structure
    // ev::io& socketIOW = _connectSocketWatchers[fd]; // newly constructed io watcher is returned
    // socketIOW.set<ConnectSocketManager, &ConnectSocketManager::_connectSocketIOCallback> (this);
    // socketIOW.start(fd, ev::WRITE);

    return fd;
}

void libBLEEP::SocketManager_v2::RemoveConnectSocket(int fd) {
    auto it = _connectSockets.find(fd);
    if ( it != _connectSockets.end())
        _connectSockets.erase(it);

    // auto it_w = _connectSocketWatchers.find(fd);
    // if ( it_w != _connectSocketWatchers.end())
    //     _connectSocketWatchers.erase(it_w);
}

