// "Copyright [2021] <kaistshadow>"

#include <memory>
#include "ProtocolLayerEx1.h"

using namespace libBLEEP_BL;

BL_ProtocolLayerEx1::BL_ProtocolLayerEx1() : BL_ProtocolLayer_API(),
                                             _txGossipProtocol(nullptr) {
    _txGossipProtocol.SetTxPool(_txPool);
}

void BL_ProtocolLayerEx1::RecvMsgHandler(std::shared_ptr<Message> msg) {
    std::cout << "recv protocol msg" << "\n";
    MessageType msgType = msg->GetType();
    if (msgType == "TXGOSSIP-INV") {
        _txGossipProtocol.RecvInventoryHandler(msg);
    } else if (msgType == "TXGOSSIP-GETDATA") {
        _txGossipProtocol.RecvGetdataHandler(msg);
    } else if (msgType == "TXGOSSIP-TXS") {
        _txGossipProtocol.RecvTxsHandler(msg);
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
    _startPeriodicTxGen(30, 30);
    return true;
}

bool BL_ProtocolLayerEx1::StopProtocol() {
    // TODO : implement
    // do nothing?
    return true;
}
