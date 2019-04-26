#ifndef SOCKET_MANAGER_V2_H
#define SOCKET_MANAGER_V2_H

#include "../datamodules/Socket_v2.h"

#include <map>
#include <memory>
#include <vector>

namespace libBLEEP {

    class SocketManager_v2 {
    private:
        std::map<int, std::shared_ptr<ListenSocket_v2> > _listenSockets;  // map fd -> ListenSocket structure

    public:
        // create listening socket + create IO watcher for it
        int CreateListenSocket(int port = DEFAULT_SOCKET_PORT);

        // should we need method for deleting socket?

        /* get ListenSocket method */
        std::shared_ptr<ListenSocket_v2> GetListenSocket(int fd);

    private:
        std::map<int, std::shared_ptr<DataSocket_v2> > _dataSockets;  // map fd -> ConnectSocket structure

    public:
        // When the CreateDataSocket is called?
        // when receiveConnection event is triggered : accept & create data socket
        // when socketConnected event is triggered : replace connect socket as data socket
        void CreateDataSocket(int sfd);

        // remove data socket from container and remove related IO watcher
        void RemoveDataSocket(int fd);

        /* get DataSocket method */
        std::shared_ptr<DataSocket_v2> GetDataSocket(int fd);
        std::vector<std::shared_ptr<DataSocket_v2> > GetAllDataSockets();

    private:
        std::map<int, std::shared_ptr<ConnectSocket_v2> > _connectSockets;  // map fd -> ConnectSocket structure

    public:
        // create connecting socket + create IO watcher for it
        int CreateNonblockConnectSocket(std::string destination);

        // remove connecting socket and related IO watcher
        void RemoveConnectSocket(int fd);

    };

}


#endif