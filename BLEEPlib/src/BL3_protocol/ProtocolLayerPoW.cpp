#include "ProtocolLayerPoW.h"

using namespace libBLEEP_BL;

BL_ProtocolLayerPoW::BL_ProtocolLayerPoW() : BL_ProtocolLayer_API() {
    _txPool = std::make_shared<TxPool>();
}

void BL_ProtocolLayerPoW::RecvMsgHandler(std::shared_ptr<Message> msg) {
    std::cout << "recv protocol msg" << "\n";
    MessageType msgType = msg->GetType();
}

void BL_ProtocolLayerPoW::SwitchAsyncEventHandler(AsyncEvent& event) {
    switch (event.GetType()) {
    case AsyncEventEnum::ProtocolRecvMsg:
        {
            std::shared_ptr<Message> msg = event.GetData().GetProtocolMsg();
            RecvMsgHandler(msg);
            break;
        }
    default:
        break;
    }
}

bool BL_ProtocolLayerPoW::InitiateProtocol() {
    if (!_initiated) {
        std::cout << "initiating ProtocolPoW" << "\n";
        _startPeriodicTxGen(0, 3);
        _initiated = true;

        return true;
    } else {
        std::cout << "already initiated protocol" << "\n";
        return false;
    }
}

bool BL_ProtocolLayerPoW::StopProtocol() {
    // do nothing?
    return true;
}
