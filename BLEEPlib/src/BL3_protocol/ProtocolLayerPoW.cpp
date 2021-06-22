#include "ProtocolLayerPoW.h"
#include "POWProtocolParameter.h"
#include "POWBlockGossipProtocolMsg.h"

//int libBLEEP_BL::txNumPerBlock = 2;
//double libBLEEP_BL::txGenStartAt = 0;
//double libBLEEP_BL::txGenInterval = 4;
//double libBLEEP_BL::miningtime = 2;
//int libBLEEP_BL::miningnodecnt = 1;

#include "shadow_memshare_interface.h"

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
        if (_txPool->GetPendingTxNum() >= txNumPerBlock) {
            if (!_powMiner.IsMining()) {
                std::shared_ptr<POWBlock> candidateBlk = _makeCandidateBlock();
                _powMiner.AsyncEmulateBlockMining(candidateBlk, 1/miningtime/miningnodecnt);
            }
        }
    } else if (msgType == "POWBLOCK-INV") {
        _RecvPOWBlockInvHandler(msg);
    } else if (msgType == "POWBLOCK-GETBLOCKS") {
        _RecvPOWBlockGetBlocksHandler(msg);
    } else if (msgType == "POWBLOCK-GETDATA") {
        _RecvPOWBlockGetDataHandler(msg);
    } else if (msgType == "POWBLOCK-BLK") {
        _RecvPOWBlockBlkHandler(msg);
    }
}

void BL_ProtocolLayerPoW::_RecvPOWBlockInvHandler(std::shared_ptr<Message> msg) {
    std::cout << "recv POW protocol msg (POWBLOCK-INV)" << "\n";

    std::shared_ptr<POWBlockGossipInventory> inv = std::static_pointer_cast<POWBlockGossipInventory>(msg->GetObject());
    auto hashes = inv->GetHashlist();
    for (auto hash : hashes) {
        std::cout << "receive block hash:" << hash << "\n";
    }

    // if the node already is processing previously received inventory, then returns immediately
    if (HasProcessingInv())
        return;

    UINT256_t lasthash = _blocktree.GetLastHash(); // for sanity check
    // if there's no block hash on BlockTree, send back 'getblocks' msg for retrieving block hashes
    // if there exists block hash but no actual block data on BlockTree, send back 'getdata' msg for retrieving actual block
    if (!_blocktree.ContainBlockHash(hashes.front())) {
        std::vector<std::string> blockLocator = _blocktree.GetBlockLocator();

        for (std::string h : blockLocator) {
            std::cout << "hash in blocklocator:" << h << "\n";
        }

        std::shared_ptr<MessageObject> ptrToObj = std::make_shared<POWBlockGossipGetBlocks>(blockLocator);
        std::shared_ptr<Message> message = std::make_shared<Message>(msg->GetSource(), "POWBLOCK-GETBLOCKS", ptrToObj);
        BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(msg->GetSource(), message);
    } else {
        // Register processing inventory
        SetProcessingInv(hashes);
        StartProcessingInv();

        // Second, if block tree lacks of actual block, request the block
        bool containAll = true;
        for (std::string h : hashes) {
            if (!_blocktree.ContainBlock(h)) {
                std::cout << "send POW getdata message" << "\n";

                std::shared_ptr<MessageObject> ptrToObj = std::make_shared<POWBlockGossipGetData>(h);
                std::shared_ptr<Message> message = std::make_shared<Message>(msg->GetSource(), "POWBLOCK-GETDATA", ptrToObj);
                BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(msg->GetSource(), message);

                containAll = false;
                break; // request only a single block
            }
            else
                std::cout << "blocktree contains" << h << "\n";
        }

        if (containAll) {
            std::cout << "stop processing inv" << "\n";
            StopProcessingInv();
        }
    }
    libBLEEP::M_Assert( lasthash == _blocktree.GetLastHash(), "Lasthash should not be changed");

}

void BL_ProtocolLayerPoW::_RecvPOWBlockGetBlocksHandler(std::shared_ptr<Message> msg) {
    std::cout << "received POW protocol getblocks message" << "\n";

    std::shared_ptr<POWBlockGossipGetBlocks> getblocks = std::static_pointer_cast<POWBlockGossipGetBlocks>(msg->GetObject());
    std::vector<std::string> blockLocator = getblocks->GetBlockLocator();
    for (auto hash : blockLocator) {
        std::cout << "blocklocator's block hash:" << hash << "\n";
    }

    std::string commonBlkHash = "";
    commonBlkHash = _blocktree.FindCommonBlock(blockLocator);
    M_Assert(commonBlkHash != "", "At least, genesis is common block");

    std::cout << "received from:" << msg->GetSource().GetId() << "\n";
    std::cout << "commonBlkHash:" << commonBlkHash << "\n";

    std::vector<std::string> chain = _blocktree.GetChainFromBlock(commonBlkHash);
    std::vector<std::string> inv;
    for (std::vector<std::string>::reverse_iterator it = chain.rbegin();
         it != chain.rend(); it++) {
        inv.push_back(*it);
    }
    // TODO: received peer should implement Initial Block Download Mechanism (IBD)

    std::cout << "inventory size:" << inv.size() << "\n";

    // create POWBLOCK-INV message
    std::shared_ptr<MessageObject> ptrToObj = std::make_shared<POWBlockGossipInventory>(inv);
    std::shared_ptr<Message> message = std::make_shared<Message>(msg->GetSource(), "POWBLOCK-INV", ptrToObj);
    BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(msg->GetSource(), message);
}

void BL_ProtocolLayerPoW::_RecvPOWBlockGetDataHandler(std::shared_ptr<Message> msg) {

    std::cout << "received POW protocol getdata message" << "\n";

    std::shared_ptr<POWBlockGossipGetData> getdata = std::static_pointer_cast<POWBlockGossipGetData>(msg->GetObject());
    std::string blockhash = getdata->GetBlockHash();
    std::cout << "getdata's block hash:" << blockhash << "\n";

    M_Assert(_blocktree.ContainBlockHash(blockhash), "hash must be synchronized already.");
    M_Assert(_blocktree.ContainBlock(blockhash), "must have blockdata.");

    // create POWBLOCK-BLK message
    std::shared_ptr<MessageObject> ptrToObj = std::make_shared<POWBlockGossipBlk>(_blocktree.GetBlock(blockhash));
    std::shared_ptr<Message> message = std::make_shared<Message>(msg->GetSource(), "POWBLOCK-BLK", ptrToObj);
    BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(msg->GetSource(), message);
}


void BL_ProtocolLayerPoW::_RecvPOWBlockBlkHandler(std::shared_ptr<Message> msg) {
    std::cout << "received POW protocol block message" << "\n";

    std::shared_ptr<POWBlockGossipBlk> getdata = std::static_pointer_cast<POWBlockGossipBlk>(msg->GetObject());
    std::shared_ptr<POWBlock> blkptr = getdata->GetBlock();

    blkptr = memshare::lookup(blkptr);

    UINT256_t lasthash = _blocktree.GetLastHash();
    // append a block to ledger
    std::cout<<"blockID:"<<blkptr->GetBlockIdx()<<"\n";
    std::cout << "blockhash:" << blkptr->GetBlockHash().str() << "\n";
    if (!_blocktree.ContainBlock(blkptr->GetBlockHash().str()))  {
        std::list<SimpleTransactionId> txids;
        for (auto tx: blkptr->GetTransactions()) {
            txids.push_back(tx->GetId());
        }
        _txPool->RemoveTxs(txids);
        _blocktree.AppendBlock(blkptr);
    }

    // if lasthash is changed, restart mining
    if (lasthash != _blocktree.GetLastHash()) {
        // stop mining
        if (_powMiner.IsMining()) {
            _powMiner.StopMining();
        }
        // restart mining for new block
        if (_txPool->GetPendingTxNum() >= txNumPerBlock) {
            std::shared_ptr<POWBlock> candidateBlk = _makeCandidateBlock();
            _powMiner.AsyncEmulateBlockMining(candidateBlk, 1/miningtime/miningnodecnt);
        }
    }

    if (HasProcessingInv()) {
        std::vector<std::string>& inv = GetProcessingInv();

        // if hash tree lacks of actual block, request the block
        // else, finish the inv processing.
        bool invAllProcessed = true;
        for (std::string h : inv) {
            if (!_blocktree.ContainBlock(h)) {
                std::cout << "sending POW protocol getdata for" << h << "\n";

                std::shared_ptr<MessageObject> ptrToObj = std::make_shared<POWBlockGossipGetData>(h);
                std::shared_ptr<Message> message = std::make_shared<Message>(msg->GetSource(), "POWBLOCK-GETDATA", ptrToObj);
                BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(msg->GetSource(), message);

                invAllProcessed = false;
                break; // request only a single block
            }
        }

        if (invAllProcessed) {
            std::cout << "stop processing inv" << "\n";
            StopProcessingInv();

            // relay POWBLOCK-INV message??
            std::shared_ptr<MessageObject> ptrToObj = std::make_shared<POWBlockGossipInventory>(inv);
            std::vector<PeerId> neighborIds = BL_PeerConnectivityLayer_API::Instance()->GetNeighborPeerIds();
            for (auto neighborId : neighborIds) {
                if (neighborId.GetId() != msg->GetSource().GetId()) {
                    std::cout << "send(relay) inv to " << neighborId.GetId() << "\n";
                    std::shared_ptr<Message> message = std::make_shared<Message>(neighborId, "POWBLOCK-INV", ptrToObj);
                    BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(neighborId, message);
                }
            }
        }
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

            memshare::try_share(minedBlk);
            minedBlk = memshare::lookup(minedBlk);

            // append block to ledger
            _blkPool.push_back(minedBlk);
            _blocktree.AppendBlock(minedBlk);

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
        _startPeriodicTxGen(10, 1);
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

        miningtime = powparams->miningtime;
        miningnodecnt = powparams->miningnodecnt;
        std::cout << "miningtime:" << miningtime << "\n";
        std::cout << "miningnodecnt:" << miningnodecnt << "\n";

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
