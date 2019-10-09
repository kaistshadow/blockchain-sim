#ifndef RECV_BUFF_MANAGER_H
#define RECV_BUFF_MANAGER_H

#include <string>
#include <map>
#include <memory>

namespace libBLEEP_BL {
    class SocketRecvBuffer {
    public:
        SocketRecvBuffer() { received_len = 0; recv_str = ""; }
        int received_len; // TODO : This member variable has ambiguous meaning.
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

#endif
