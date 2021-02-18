//
// Created by ilios on 21. 2. 15..
//
#include <iostream>
#include "BLNodePrimitives.h"

#include "BL2_peer_connectivity/Message.h"
#include "BL2_peer_connectivity/Peer.h"

using namespace libBLEEP_sybil;

void BLNodePrimitives::OpAfterConnect() {
    std::cout << "OpAfterConnect for node [" << _node->GetIP() << "]" << "\n";

    // send initializing message

    // 1. create message

    // how to send P2P message
    // how to manage Transport layer buffer
    // how to manage asynchronous I/O events
}

void BLNodePrimitives::CallbackAfterReceive() {
    std::cout << "CallbackAfterReceive" << "\n";
}
