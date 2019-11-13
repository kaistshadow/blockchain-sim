#ifndef PROTOCOL_LAYER_API_H
#define PROTOCOL_LAYER_API_H

#include "BL_MainEventManager.h"

namespace libBLEEP_BL {
    class BL_ProtocolLayer_API {
    public:
        BL_ProtocolLayer_API() {};
        
        ~BL_ProtocolLayer_API() {};
        
        /* Switch asynchronous event to proper handler */
        virtual void SwitchAsyncEventHandler(AsyncEvent& event) = 0;


        /* public API functions */
        virtual bool InitiateProtocol() = 0;
        virtual bool StopProtocol() = 0;

    };

    extern std::unique_ptr<BL_ProtocolLayer_API> g_ProtocolLayer_API;
}


#endif
