#ifndef PROTOCOL_LAYER_API_H
#define PROTOCOL_LAYER_API_H

#include "BL_MainEventManager.h"

namespace libBLEEP_BL {
    class BL_ProtocolLayer_API {
        // singleton pattern
    public:
        static BL_ProtocolLayer_API *Instance();

        static void InitInstance(std::string protocolType);

    protected:
        BL_ProtocolLayer_API();

    private:
        static BL_ProtocolLayer_API *_instance;
    protected:
        bool _initiated = false;

    protected:
        std::shared_ptr<TxPool> _txPool;
        int _txGenNum = 0;
        std::list<std::shared_ptr<Block>> _blkPool;
    public:
        int GetTxGeneratedNum() { return _txGenNum; }
        std::shared_ptr<TxPool> GetTxPool() { return _txPool; }
        int GetBlockPoolSize() { return _blkPool.size(); }
        std::shared_ptr<Block> GetLastBlock() { return _blkPool.back(); }

    public:

        ~BL_ProtocolLayer_API() {};

        /* Switch asynchronous event to proper handler */
        virtual void SwitchAsyncEventHandler(AsyncEvent &event) = 0;


        /* public API functions */
        virtual bool InitiateProtocol() = 0;

        virtual bool StopProtocol() = 0;

    };

}


#endif
