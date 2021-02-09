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

}

#endif //BLEEP_SOCKETLAYER_BITCOIN_H
