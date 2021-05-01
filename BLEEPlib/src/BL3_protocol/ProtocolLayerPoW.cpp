#include "ProtocolLayerPoW.h"
#include "POWProtocolParameter.h"
#include "POWBlockGossipProtocolMsg.h"

int libBLEEP_BL::txNumPerBlock = 2;
double libBLEEP_BL::txGenStartAt = 0;
double libBLEEP_BL::txGenInterval = 4;
double libBLEEP_BL::miningtime = 2;
int libBLEEP_BL::miningnodecnt = 1;

using namespace libBLEEP_BL;


BL_ProtocolLayerPoW::BL_ProtocolLayerPoW() : BL_ProtocolLayer_API(),
                                             _txGossipProtocol(_txPool) {
}

void BL_ProtocolLayerPoW::RecvMsgHandler(std::shared_ptr<Message> msg) {
    std::cout << "recv protocol msg" << "\n";
    MessageType msgType = msg->GetType();
    if (msgType == "TXGOSSIP-INV") {
        _txGossipProtocol.RecvInventoryHandler(msg);
    } else if (msgType == "TXGOSSIP-GETDATA") {
        _txGossipProtocol.RecvGetdataHandler(msg);
    } else if (msgType == "TXGOSSIP-TXS") {
        _txGossipProtocol.RecvTxsHandler(msg);
    } else if (msgType == "POWBLOCK-INV") {
        _RecvPOWBlockInvHandler(msg);
    }
}

void BL_ProtocolLayerPoW::_RecvPOWBlockInvHandler(std::shared_ptr<Message> msg) {
    std::cout << "recv POW protocol msg (POWBLOCK-INV)" << "\n";

    std::shared_ptr<POWBlockGossipInventory> inv = std::static_pointer_cast<POWBlockGossipInventory>(msg->GetObject());
    auto hashes = inv->GetHashlist();
    for (auto hash : hashes) {
        std::cout << "receive block hash:" << hash << "\n";
    }
}

void BL_ProtocolLayerPoW::SwitchAsyncEventHandler(AsyncEvent& event) {
    switch (event.GetType()) {
        case AsyncEventEnum::ProtocolRecvMsg:
        {
            std::shared_ptr<Message> msg = event.GetData().GetProtocolMsg();
            RecvMsgHandler(msg);
            break;
        }
        case AsyncEventEnum::EmuBlockMiningComplete:
        {
            std::cout << "block mining complete" << "\n";
            std::shared_ptr<POWBlock> minedBlk = event.GetData().GetMinedBlock();
            std::cout << "blockhash:" << minedBlk->GetBlockHash() << "\n";
            std::cout << "blockhash(str):" << minedBlk->GetBlockHash().str() << "\n";
            std::cout << "blockhash:" << libBLEEP::UINT256_t((const unsigned char*)minedBlk->GetBlockHash().str().c_str(), 32) << "\n";

            // append block to ledger
            _blkPool.push_back(minedBlk);

            // remove tx from txpool
            std::list<SimpleTransactionId> txids;
            for (auto tx: minedBlk->GetTransactions()) {
                txids.push_back(tx->GetId());
            }
            _txPool->RemoveTxs(txids);

            std::vector<std::string> hashes;
            hashes.push_back(minedBlk->GetBlockHash().str());
            std::shared_ptr<MessageObject> ptrToObj = std::make_shared<POWBlockGossipInventory>(hashes);

            std::vector<PeerId> neighborIds = BL_PeerConnectivityLayer_API::Instance()->GetNeighborPeerIds();
            for (auto &neighborId : neighborIds) {
                std::cout << "send POWBlockinv to " << neighborId.GetId() << "\n";
                std::shared_ptr<Message> message = std::make_shared<Message>(neighborId, "POWBLOCK-INV", ptrToObj);
                BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(neighborId, message);
            }

            break;
        }
    default:
        break;
    }
}

bool BL_ProtocolLayerPoW::InitiateProtocol() {
    if (!_initiated) {
        std::cout << "initiating ProtocolPoW" << "\n";
        _startPeriodicTxGen(libBLEEP_BL::txGenStartAt, libBLEEP_BL::txGenInterval);
        _initiated = true;

        return true;
    } else {
        std::cout << "already initiated protocol" << "\n";
        return false;
    }
}

bool BL_ProtocolLayerPoW::InitiateProtocol(ProtocolParameter* params) {
//    POWProtocolParameter& powparams = dynamic_cast<POWProtocolParameter &>(params);
    POWProtocolParameter* powparams = dynamic_cast<POWProtocolParameter*>(params);
    assert(powparams != nullptr);
    if (!_initiated) {
        std::cout << "initiating ProtocolPoW with custom params" << "\n";
        _startPeriodicTxGen(powparams->txGenStartAt, powparams->txGenInterval);
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
