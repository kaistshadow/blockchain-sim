// "Copyright [2021] <kaistshadow>"

#ifndef BLEEPLIB_SRC_BL3_PROTOCOL_TXGOSSIPPROTOCOL_H_
#define BLEEPLIB_SRC_BL3_PROTOCOL_TXGOSSIPPROTOCOL_H_

#include <memory>
#include <vector>
#include <iostream>
#include <boost/serialization/vector.hpp>

#include "TxPool.h"
#include "Transaction.h"

#include "../BL2_peer_connectivity/PeerConnectivityLayer_API.h"
#include "../BL2_peer_connectivity/Peer.h"
#include "../BL2_peer_connectivity/MessageObject.h"

namespace libBLEEP_BL {
class Message;

class TxGossipProtocol {
 private:
    std::shared_ptr<TxPool> _txPool;
    std::vector<std::shared_ptr<SimpleTransaction> > _txToBroadcast;

 private:
    // periodic tx broadcast
    ev::timer _txbctimer;
    void _txbroadcasttimerCallback(ev::timer &w, int revents) {
        if (_txToBroadcast.empty())
            return;

        std::vector<SimpleTransactionId> txids;
        for (auto tx : _txToBroadcast)
            txids.push_back(tx->GetId());

        std::vector<PeerId> neighborIds = BL_PeerConnectivityLayer_API::Instance()->GetNeighborPeerIds();
        for (auto &neighborId : neighborIds) {
            std::cout << "send inv to " << neighborId.GetId() << "\n";
            std::shared_ptr<MessageObject> ptrToObj = std::make_shared<TxGossipInventory>(txids);
            std::shared_ptr<Message> message = std::make_shared<Message>(neighborId, "TXGOSSIP-INV", ptrToObj);
            BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(neighborId, message);
        }
        _txToBroadcast.clear();
    }
    void _startPeriodicTxBroadcast(double start, double interval) {
        _txbctimer.set<TxGossipProtocol, &TxGossipProtocol::_txbroadcasttimerCallback>(this);
        _txbctimer.set(start, interval);
        _txbctimer.start();
    }

 public:
    TxGossipProtocol(std::shared_ptr<TxPool> txPool) {
        _txPool = txPool;
        _startPeriodicTxBroadcast(30, 30);
    }

    void SetTxPool(std::shared_ptr<TxPool> txPool) { _txPool = txPool; }
    void PushTxToBroadcast(std::shared_ptr<SimpleTransaction> tx) { _txToBroadcast.push_back(tx); }

    void RecvInventoryHandler(std::shared_ptr<Message> msg);

    void RecvGetdataHandler(std::shared_ptr<Message> msg);

    void RecvTxsHandler(std::shared_ptr<Message> msg);
};
}  // namespace libBLEEP_BL

#endif  // BLEEPLIB_SRC_BL3_PROTOCOL_TXGOSSIPPROTOCOL_H_
