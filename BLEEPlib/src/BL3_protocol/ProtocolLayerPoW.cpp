#include "ProtocolLayerPoW.h"

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

bool BL_ProtocolLayerPoW::StopProtocol() {
    // do nothing?
    return true;
}
