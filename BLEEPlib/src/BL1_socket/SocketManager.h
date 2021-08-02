// "Copyright [2021] <kaistshadow>"

#ifndef BLEEPLIB_SRC_BL1_SOCKET_SOCKETMANAGER_H_
#define BLEEPLIB_SRC_BL1_SOCKET_SOCKETMANAGER_H_

#include <map>
#include <string>
#include <memory>
#include "Socket.h"

namespace libBLEEP_BL {

class SocketManager {
 public:
    ~SocketManager() { std::cout << "SocketManager Destructor called" << "\n"; }

 private:
    std::map<int, std::shared_ptr<ListenSocket> > _listenSockets;

 public:
    // create listening socket + create IO watcher for it
    int CreateListenSocket(int port = DEFAULT_SOCKET_PORT);

    int CreateListenSocketForShadowIP(int port, const char *shadow_ip_addr);

    /* get ListenSocket method */
    std::shared_ptr<ListenSocket> GetListenSocket(int fd);

    void CloseAllListenSocket() { _listenSockets.clear(); }

 private:
    std::map<int, std::shared_ptr<ConnectSocket> > _connectSockets;  // map fd -> ConnectSocket structure

 public:
    // create connecting socket + create IO watcher for it
    int CreateNonblockConnectSocket(std::string destination);

    // remove connecting socket and related IO watcher
    void RemoveConnectSocket(int fd);

    std::shared_ptr<ConnectSocket> GetConnectSocket(int fd);

 private:
    std::map<int, std::shared_ptr<DataSocket> > _dataSockets;  // map fd -> DataSocket structure

 public:
    // When the CreateDataSocket is called?
    // when SocketAccept event is triggered : accept & create data socket
    // when socketConnect event is triggered : replace connect socket as data socket
    void CreateDataSocket(int sfd);

    // remove data socket from container and remove related IO watcher
    void RemoveDataSocket(int fd);

    /* get DataSocket method */
    std::shared_ptr<DataSocket> GetDataSocket(int fd);
};

} // namespace libBLEEP_BL

#endif // BLEEPLIB_SRC_BL1_SOCKET_SOCKETMANAGER_H_
