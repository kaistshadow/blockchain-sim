#ifndef TX_GOSSIP_PROTOCOL_H
#define TX_GOSSIP_PROTOCOL_H

#include <iostream>
#include "../BL2_peer_connectivity/PeerConnectivityLayer_API.h"

#include "Inventory.h"

namespace libBLEEP_BL {
    class TxGossipProtocol {
    private:
        
    public:
        TxGossipProtocol() {}
        
        void RecvInventoryHandler() {
            std::cout << "recv inventory msg" << "\n";
        }

        
    };
}


#endif
