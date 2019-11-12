#include "ProtocolLayerEx1.h"

using namespace libBLEEP_BL;

void BL_ProtocolLayerEx1::RecvMsgHandler(std::shared_ptr<Message> msg) {
    std::cout << "recv protocol msg" << "\n";
    MessageType msgType = msg->GetType();
    if (msgType == "TXGOSSIP-INV") {
        _txGossipProtocol.RecvInventoryHandler();
    }
}

void BL_ProtocolLayerEx1::SwitchAsyncEventHandler(AsyncEvent& event) {
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

bool BL_ProtocolLayerEx1::InitiateProtocol() {
    // TODO : implement

    _startPeriodicTxGen(10, 10);
    return true;
}

bool BL_ProtocolLayerEx1::StopProtocol() {
    // TODO : implement 

    // do nothing?
    return true;
}
