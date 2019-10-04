#ifndef SOCKET_LAYER_API_H
#define SOCKET_LAYER_API_H

#include "../BL_MainEventManager.h"

namespace libBLEEP_BL {
    class BL_SocketLayer_API {
    public:
        BL_SocketLayer_API() {};

        ~BL_SocketLayer_API() {};
    
        /* Switch asynchronous event to proper handler */
        virtual void SwitchAsyncEventHandler(AsyncEvent& event) = 0;

        /* public API functions */
        virtual void ConnectSocket(std::string dest) = 0;

    };

    extern std::unique_ptr<BL_SocketLayer_API> g_SocketLayer_API;
}




#endif
