#ifndef SOCKET_LAYER_API_H
#define SOCKET_LAYER_API_H

#include "../BL_MainEventManager.h"

#include "SocketManager.h"
#include "RecvBufferManager.h"

namespace libBLEEP_BL {
    class BL_SocketLayer_API {
        // singleton pattern
    public:
        static BL_SocketLayer_API *Instance();

    protected:
        BL_SocketLayer_API();

        static BL_SocketLayer_API *_instance;


    public:
        virtual ~BL_SocketLayer_API() {};

        /* Switch asynchronous event to proper handler */
        virtual void SwitchAsyncEventHandler(AsyncEvent &event);

        /* public API functions */
        virtual int ConnectSocket(std::string dest);

        virtual void AbandonConnectSocket(int fd);

        virtual void SendToSocket(int fd, const char *buf, int size);

        virtual void DisconnectSocket(int fd);

    protected:
        SocketManager _socketManager;
        RecvBufferManager _recvBuffManager;

        /* handler functions for each asynchronous event */
        virtual void AcceptHandler(int fd);

        virtual void ConnectHandler(int fd);

        virtual void RecvHandler(int fd);

        virtual void WriteHandler(int fd);

        virtual void ConnectFailedHandler(int fd, std::string domain);
    };

}




#endif
