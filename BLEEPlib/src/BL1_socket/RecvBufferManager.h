// "Copyright [2021] <kaistshadow>"
#ifndef BLEEPLIB_SRC_BL1_SOCKET_RECVBUFFERMANAGER_H_
#define BLEEPLIB_SRC_BL1_SOCKET_RECVBUFFERMANAGER_H_

#include <string>
#include <map>
#include <memory>

namespace libBLEEP_BL {
class SocketRecvBuffer {
 public:
    SocketRecvBuffer() { received_len = 0; recv_str = ""; }
    int received_len;
    std::string recv_str;
};

class RecvBufferManager {
 private:
    std::map<int, std::shared_ptr<SocketRecvBuffer> > _recvBuffMap; // map fd -> SocketRecvBuffer structure
 public:
    void AppendToBuffer(int fd, char* buf, int size);
    std::shared_ptr<SocketRecvBuffer> GetRecvBuffer(int fd);
    void RemoveRecvBuffer(int fd);
};

}

#endif // BLEEPLIB_SRC_BL1_SOCKET_RECVBUFFERMANAGER_H_
