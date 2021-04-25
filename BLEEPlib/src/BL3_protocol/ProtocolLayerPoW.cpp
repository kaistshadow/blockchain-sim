#include "ProtocolLayerPoW.h"

using namespace libBLEEP_BL;

BL_ProtocolLayerPoW::BL_ProtocolLayerPoW() : BL_ProtocolLayer_API() {

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
    std::cout << "initiating ProtocolPoW" << "\n";

    return true;
}

bool BL_ProtocolLayerPoW::StopProtocol() {
    // do nothing?
    return true;
}
