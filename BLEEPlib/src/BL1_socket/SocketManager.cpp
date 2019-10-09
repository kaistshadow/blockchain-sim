#include "SocketManager.h"


using namespace libBLEEP_BL;

int SocketManager::CreateListenSocket(int port) {
    // new ListenSocket
    std::shared_ptr<ListenSocket> new_socket = std::make_shared<ListenSocket>(port);
    _listenSockets[new_socket->GetFD()] = new_socket;

    return new_socket->GetFD();
}

std::shared_ptr<ListenSocket> SocketManager::GetListenSocket(int fd) {
    auto it = _listenSockets.find(fd);
    if (it == _listenSockets.end()) {
        std::cout << "No valid listenSocket exists" << "\n";
        return nullptr;
    }
    return it->second;
}

int SocketManager::CreateNonblockConnectSocket(std::string dest) {
    // new ConnectSocket
    std::shared_ptr<ConnectSocket> new_socket = std::make_shared<ConnectSocket>(dest);
    _connectSockets[new_socket->GetFD()] = new_socket;

    int fd = new_socket->GetFD();

    return fd;
}

void SocketManager::RemoveConnectSocket(int fd) {
    auto it = _connectSockets.find(fd);
    if ( it != _connectSockets.end())
        _connectSockets.erase(it);

}

std::shared_ptr<ConnectSocket> SocketManager::GetConnectSocket(int fd) {
    auto it = _connectSockets.find(fd);
    if (it == _connectSockets.end()) {
        std::cout << "No valid connectSocket exists" << "\n";
        return nullptr;
    }
    return it->second;
}


void SocketManager::CreateDataSocket(int sfd) {
    // new DataSocket
    std::shared_ptr<DataSocket> new_socket = std::make_shared<DataSocket>(sfd);
    _dataSockets[new_socket->GetFD()] = new_socket;
}

void SocketManager::RemoveDataSocket(int fd) {
    auto it = _dataSockets.find(fd);
    if ( it != _dataSockets.end())
        _dataSockets.erase(it);

}

std::shared_ptr<DataSocket> SocketManager::GetDataSocket(int fd) {
    auto it = _dataSockets.find(fd);
    if (it == _dataSockets.end()) {
        std::cout << "No valid dataSocket exists" << "\n";
        return nullptr;
    }
    return it->second;
}
