//
// Created by ilios on 21. 1. 26..
//
#include "ProtocolLayer_API.h"
#include "ProtocolLayerEx1.h"
#include "ProtocolLayerPoW.h"
#include "ProtocolLayerPoS.h"
#include "utility/Assert.h"
#include <iostream>

#include "shadow_interface.h"

using namespace libBLEEP_BL;

BL_ProtocolLayer_API *BL_ProtocolLayer_API::_instance = 0;

BL_ProtocolLayer_API *BL_ProtocolLayer_API::Instance() {
    if (_instance == 0) {
        libBLEEP::M_Assert(0, "Protocol Instance must be initialized first.");
    }
    return _instance;
}

void BL_ProtocolLayer_API::InitInstance(std::string protocolType) {
    std::cout << "Protocol Layer initialization" << "\n";
    libBLEEP::M_Assert(_instance == 0, "Protocol Instance must be initialized once.");
    if (protocolType == "Ex1")
        _instance = new BL_ProtocolLayerEx1();
    else if (protocolType == "PoW")
        _instance = new BL_ProtocolLayerPoW();
    else if (protocolType == "PoS")
        _instance = new BL_ProtocolLayerPoS();
    else // default protocol
        _instance = new BL_ProtocolLayerEx1();
}

BL_ProtocolLayer_API::BL_ProtocolLayer_API() {
    _txPool = std::make_shared<TxPool>();
}