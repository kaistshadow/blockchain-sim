#ifndef SOCKET_LAYER_API_H
#define SOCKET_LAYER_API_H

#include "../BL_MainEventManager.h"

namespace libBLEEP_BL {
    class BL_SocketLayer_API {
    public:
        BL_SocketLayer_API() {};

        virtual ~BL_SocketLayer_API() {};

        /* Switch asynchronous event to proper handler */
        virtual void SwitchAsyncEventHandler(AsyncEvent &event) = 0;

        /* public API functions */
        virtual int ConnectSocket(std::string dest) = 0;

        virtual void SendToSocket(int fd, const char *buf, int size) = 0;

        virtual void DisconnectSocket(int fd) = 0;

        /* for managing Shadow IPs */
        virtual void CreateSocketForShadowIP(int port, const char *shadow_ip_addr) {};
    };

    extern std::unique_ptr<BL_SocketLayer_API> g_SocketLayer_API;
}




#endif
