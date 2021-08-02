// "Copyright [2021] <kaistshadow>"
//
// Created by ilios on 20. 8. 26..
//

#ifndef BLEEPLIB_SRC_BL1_SOCKET_SOCKETLAYER_BITCOIN_H_
#define BLEEPLIB_SRC_BL1_SOCKET_SOCKETLAYER_BITCOIN_H_



#include "SocketLayer_API.h"
#include "SocketManager.h"
#include "RecvBufferManager.h"
#include "../BL_MainEventManager.h"

#define BITCOIN_MAGIC "\xf9\xbe\xb4\xd9"
#define BITCOIN_MAGIC_SIZE 4
#define BITCOIN_COMMAND_SIZE 12

namespace libBLEEP_BL {

class BL_SocketLayer_Bitcoin: public BL_SocketLayer_API {
 protected:
    BL_SocketLayer_Bitcoin();

 public:
    static void RegisterInstance();

    static BL_SocketLayer_Bitcoin *Instance();

    /* public API functions */
    void CreateSocketForShadowIP(int port, const char *shadow_ip_addr);


 protected:
    void RecvHandler(int fd);
};

} // namespace libBLEEP_BL

#endif // BLEEPLIB_SRC_BL1_SOCKET_SOCKETLAYER_BITCOIN_H_
