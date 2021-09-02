// "Copyright [2021] <kaistshadow>"
#include "RecvBufferManager.h"
#include <iostream>

using namespace libBLEEP_BL;

void RecvBufferManager::AppendToBuffer(int fd, char* buf, int size) {
    auto it = _recvBuffMap.find(fd);
    if (it == _recvBuffMap.end()) {
        // create new SocketRecvBuffer
        std::shared_ptr<SocketRecvBuffer> buffer = std::make_shared<SocketRecvBuffer>();
        buffer->recv_str.append(buf, size);
        buffer->received_len += size;
        _recvBuffMap[fd] = buffer;
    } else {
        std::shared_ptr<SocketRecvBuffer> buffer = it->second;
        buffer->recv_str.append(buf, size);
        buffer->received_len += size;
    }
}

std::shared_ptr<SocketRecvBuffer> RecvBufferManager::GetRecvBuffer(int fd) {
    auto it = _recvBuffMap.find(fd);
    if (it == _recvBuffMap.end()) {
        std::cout << "No valid recvBuffer for given fd:" << fd << "\n";
        return nullptr;
    }
    return it->second;
}

void RecvBufferManager::RemoveRecvBuffer(int fd) {
    auto it = _recvBuffMap.find(fd);
    if ( it != _recvBuffMap.end())
        _recvBuffMap.erase(it);
}
