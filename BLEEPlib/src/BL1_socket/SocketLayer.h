#ifndef SOCKET_LAYER_H
#define SOCKET_LAYER_H

#include "../BL_MainEventManager.h"

#include "SocketLayer_API.h"
#include "SocketManager.h"


namespace libBLEEP_BL {

    class BL_SocketLayer: public BL_SocketLayer_API {
    private:
        SocketManager _socketManager;

        /* handler functions for each asynchronous event */
        void AcceptHandler(int fd);
        void ConnectHandler(int fd);

    public:
        BL_SocketLayer();

        /* Switch asynchronous event to proper handler */
        virtual void SwitchAsyncEventHandler(AsyncEvent& event);


        /* public API functions */
        virtual void ConnectSocket(std::string dest);
    };

}

#endif
