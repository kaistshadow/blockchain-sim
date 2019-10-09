#ifndef SOCKET_LAYER_H
#define SOCKET_LAYER_H

#include "../BL_MainEventManager.h"

#include "SocketLayer_API.h"
#include "SocketManager.h"
#include "RecvBufferManager.h"


namespace libBLEEP_BL {

    class BL_SocketLayer: public BL_SocketLayer_API {
    private:
        SocketManager _socketManager;
        RecvBufferManager _recvBuffManager;

        /* handler functions for each asynchronous event */
        void AcceptHandler(int fd);
        void ConnectHandler(int fd);
        void RecvHandler(int fd);
        void WriteHandler(int fd);

    public:
        BL_SocketLayer();

        /* Switch asynchronous event to proper handler */
        virtual void SwitchAsyncEventHandler(AsyncEvent& event);


        /* public API functions */
        virtual int ConnectSocket(std::string dest); // return fd
        virtual void SendToSocket(int fd, char* buf, int size);
    };

}

#endif
