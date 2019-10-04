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


