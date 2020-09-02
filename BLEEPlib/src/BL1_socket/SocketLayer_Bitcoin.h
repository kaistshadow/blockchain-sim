//
// Created by ilios on 20. 8. 26..
//

#ifndef BLEEP_SOCKETLAYER_BITCOIN_H
#define BLEEP_SOCKETLAYER_BITCOIN_H


#include "../BL_MainEventManager.h"

#include "SocketLayer_API.h"
#include "SocketManager.h"
#include "RecvBufferManager.h"

#define BITCOIN_MAGIC "\xf9\xbe\xb4\xd9"
#define BITCOIN_MAGIC_SIZE 4
#define BITCOIN_COMMAND_SIZE 12

namespace libBLEEP_BL {

    class BL_SocketLayer_Bitcoin: public BL_SocketLayer_API {
    private:
        SocketManager _socketManager;
        RecvBufferManager _recvBuffManager;

        /* handler functions for each asynchronous event */
        void AcceptHandler(int fd);
        void ConnectHandler(int fd);
        void RecvHandler(int fd);
        void WriteHandler(int fd);

    public:
        BL_SocketLayer_Bitcoin();

        /* Switch asynchronous event to proper handler */
        virtual void SwitchAsyncEventHandler(AsyncEvent& event);


        /* public API functions */
        virtual int ConnectSocket(std::string dest); // return fd

        virtual void SendToSocket(int fd, const char* buf, int size);
        virtual void DisconnectSocket(int fd);
    };

}

#endif //BLEEP_SOCKETLAYER_BITCOIN_H
