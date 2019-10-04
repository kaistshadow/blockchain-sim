#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include <map>
#include "Socket.h"

namespace libBLEEP_BL {

    class SocketManager {
    private:
        std::map<int, std::shared_ptr<ListenSocket> > _listenSockets;  // map fd -> ListenSocket structure        

    public:
        // create listening socket + create IO watcher for it
        int CreateListenSocket(int port = DEFAULT_SOCKET_PORT);

        /* get ListenSocket method */
        std::shared_ptr<ListenSocket> GetListenSocket(int fd);

    private:
        std::map<int, std::shared_ptr<ConnectSocket> > _connectSockets;  // map fd -> ConnectSocket structure

    public:
        // create connecting socket + create IO watcher for it
        int CreateNonblockConnectSocket(std::string destination);

        // remove connecting socket and related IO watcher
        void RemoveConnectSocket(int fd);
    };

}



#endif
