#include "TxGossipProtocol.h"

#include "../BL2_peer_connectivity/Message.h"
#include "../BL2_peer_connectivity/PeerConnectivityLayer_API.h"

using namespace libBLEEP_BL;

void TxGossipProtocol::RecvInventoryHandler(std::shared_ptr<Message> msg) {
    std::cout << "recv inventory msg" << "\n";
    std::shared_ptr<TxGossipInventory> inv = std::static_pointer_cast<TxGossipInventory>(msg->GetObject());
    auto tids = inv->GetTransactionIds();
    std::vector<SimpleTransactionId> getdataIds;
    for (auto tid : tids) {
        std::cout << "receive transaction id:" << tid << "\n";
        
        // check whether the txpool has the tx
        if (!_txPool->ContainTx(tid))
            getdataIds.push_back(tid);
    }
    if (!getdataIds.empty()) {
        // send getdata
        std::shared_ptr<MessageObject> ptrToObj = std::make_shared<TxGossipGetdata>(getdataIds);
        std::shared_ptr<Message> message = std::make_shared<Message>(msg->GetSource(), "TXGOSSIP-GETDATA", ptrToObj);
        BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(msg->GetSource(), message);
    }
}

void TxGossipProtocol::RecvGetdataHandler(std::shared_ptr<Message> msg) {
    std::cout << "recv getdata msg" << "\n";
    std::shared_ptr<TxGossipGetdata> getdata = std::static_pointer_cast<TxGossipGetdata>(msg->GetObject());
    auto tids = getdata->GetTransactionIds();
    std::vector<SimpleTransaction> txs;
    for (auto tid : tids) {
        std::cout << "receive getdata tx id:" << tid << "\n";
        
        // check whether the txpool has the tx
        if (_txPool->ContainTx(tid))
            txs.push_back(*_txPool->GetTx(tid));
    }
    if (!txs.empty()) {
        // send txs
        std::shared_ptr<MessageObject> ptrToObj = std::make_shared<TxGossipTxs>(txs);
        std::shared_ptr<Message> message = std::make_shared<Message>(msg->GetSource(), "TXGOSSIP-TXS", ptrToObj);
        BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(msg->GetSource(), message);
    }
}

void TxGossipProtocol::RecvTxsHandler(std::shared_ptr<Message> msg) {
    std::cout << "recv txs msg" << "\n";
    std::shared_ptr<TxGossipTxs> txsObject = std::static_pointer_cast<TxGossipTxs>(msg->GetObject());
    auto txs = txsObject->GetTransactions();
    for (auto tx : txs) {
        std::cout << "receive tx:" << tx << "\n";
        
        // Add to txpool
        _txPool->AddTx(std::make_shared<SimpleTransaction>(tx));
    }

    // relay tx to neighbors
    std::vector<SimpleTransactionId> txids;
    for (auto tx : txs) {
        txids.push_back(tx.GetId());
    }
    std::vector<PeerId> neighborIds = BL_PeerConnectivityLayer_API::Instance()->GetNeighborPeerIds();
    for (auto neighborId : neighborIds) {
        if (neighborId.GetId() != msg->GetSource().GetId()) {
            std::cout << "send(relay) inv to " << neighborId.GetId() << "\n";
            std::shared_ptr<MessageObject> ptrToObj = std::make_shared<TxGossipInventory>(txids);
            std::shared_ptr<Message> message = std::make_shared<Message>(neighborId, "TXGOSSIP-INV", ptrToObj);
            BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(neighborId, message);
        }
    }

}
