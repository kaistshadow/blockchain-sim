#include "ProtocolLayerPoS.h"
#include "POSProtocolParameter.h"
#include "POSBlockGossipProtocolMsg.h"

#include <random>

using namespace libBLEEP_BL;


BL_ProtocolLayerPoS::BL_ProtocolLayerPoS() : BL_ProtocolLayer_API(),
                                             _txGossipProtocol(_txPool) {
}

void BL_ProtocolLayerPoS::RecvMsgHandler(std::shared_ptr<Message> msg) {
    std::cout << "recv protocol msg" << "\n";
    MessageType msgType = msg->GetType();
    if (msgType == "TXGOSSIP-INV") {
        _txGossipProtocol.RecvInventoryHandler(msg);
    } else if (msgType == "TXGOSSIP-GETDATA") {
        _txGossipProtocol.RecvGetdataHandler(msg);
    } else if (msgType == "TXGOSSIP-TXS") {
        _txGossipProtocol.RecvTxsHandler(msg);
    } else if (msgType == "POSBLOCK-INV") {
        _RecvPOSBlockInvHandler(msg);
    } else if (msgType == "POSBLOCK-GETBLOCKS") {
        _RecvPOSBlockGetBlocksHandler(msg);
    } else if (msgType == "POSBLOCK-GETDATA") {
        _RecvPOSBlockGetDataHandler(msg);
    } else if (msgType == "POSBLOCK-BLK") {
        _RecvPOSBlockBlkHandler(msg);
    }
}

// processing on POS Block inventory reception: request single block(GETDATA) or multiple blocks' inv(GETBLOCKS)
// same with the PoW's handler
void BL_ProtocolLayerPoS::_RecvPOSBlockInvHandler(std::shared_ptr<Message> msg) {
    std::cout << "recv POS protocol msg (POSBLOCK-INV)" << "\n";

    std::shared_ptr<POSBlockGossipInventory> inv = std::static_pointer_cast<POSBlockGossipInventory>(msg->GetObject());
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

        std::shared_ptr<MessageObject> ptrToObj = std::make_shared<POSBlockGossipGetBlocks>(blockLocator);
        std::shared_ptr<Message> message = std::make_shared<Message>(msg->GetSource(), "POSBLOCK-GETBLOCKS", ptrToObj);
        BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(msg->GetSource(), message);
    } else {
        // Register processing inventory
        SetProcessingInv(hashes);
        StartProcessingInv();

        // Second, if block tree lacks of actual block, request the block
        bool containAll = true;
        for (std::string h : hashes) {
            if (!_blocktree.ContainBlock(h)) {
                std::cout << "send POS getdata message" << "\n";

                std::shared_ptr<MessageObject> ptrToObj = std::make_shared<POSBlockGossipGetData>(h);
                std::shared_ptr<Message> message = std::make_shared<Message>(msg->GetSource(), "POSBLOCK-GETDATA", ptrToObj);
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

// processing on reception of requesting invs: send multiple invs corresponding to the request
// same with the PoW's handler
void BL_ProtocolLayerPoS::_RecvPOSBlockGetBlocksHandler(std::shared_ptr<Message> msg) {
    std::cout << "received POS protocol getblocks message" << "\n";

    std::shared_ptr<POSBlockGossipGetBlocks> getblocks = std::static_pointer_cast<POSBlockGossipGetBlocks>(msg->GetObject());
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

    // create POSBLOCK-INV message
    std::shared_ptr<MessageObject> ptrToObj = std::make_shared<POSBlockGossipInventory>(inv);
    std::shared_ptr<Message> message = std::make_shared<Message>(msg->GetSource(), "POSBLOCK-INV", ptrToObj);
    BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(msg->GetSource(), message);
}

// processing on reception of requesting block data: send the corresponding block data
// same with the PoW's handler
void BL_ProtocolLayerPoS::_RecvPOSBlockGetDataHandler(std::shared_ptr<Message> msg) {

    std::cout << "received POS protocol getdata message" << "\n";

    std::shared_ptr<POSBlockGossipGetData> getdata = std::static_pointer_cast<POSBlockGossipGetData>(msg->GetObject());
    std::string blockhash = getdata->GetBlockHash();
    std::cout << "getdata's block hash:" << blockhash << "\n";

    M_Assert(_blocktree.ContainBlockHash(blockhash), "hash must be synchronized already.");
    M_Assert(_blocktree.ContainBlock(blockhash), "must have blockdata.");

    // create POSBLOCK-BLK message
    std::shared_ptr<MessageObject> ptrToObj = std::make_shared<POSBlockGossipBlk>(_blocktree.GetBlock(blockhash));
    std::shared_ptr<Message> message = std::make_shared<Message>(msg->GetSource(), "POSBLOCK-BLK", ptrToObj);
    BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(msg->GetSource(), message);
}

// processing on reception of block data: check block validity, add block, update besttip for mining
void BL_ProtocolLayerPoS::_RecvPOSBlockBlkHandler(std::shared_ptr<Message> msg) {
    std::cout << "received POS protocol block message" << "\n";

    std::shared_ptr<POSBlockGossipBlk> getdata = std::static_pointer_cast<POSBlockGossipBlk>(msg->GetObject());
    std::shared_ptr<POSBlock> blkptr = getdata->GetBlock();

    if (!(random_selection(blkptr->GetSlotNo()) == blkptr->GetCreator())) {
        return; // mismatch between block creator and expected block creator
    }

    // add block on blockchain, whether or not the block is main-chain family.
    std::cout << "blockhash:" << blkptr->GetBlockHash().str() << "\n";
    if (!_blocktree.ContainBlock(blkptr->GetBlockHash().str())) {
        _blocktree.AppendBlock(blkptr);
        // Future TODO: every blockchain system has different besttip rule, so it might be separated from BlockTree code,
        // and make the BlockTree code to be generalized form.
    }

    if (HasProcessingInv()) {
        std::vector<std::string>& inv = GetProcessingInv();

        // if hash tree lacks of actual block, request the block
        // else, finish the inv processing.
        bool invAllProcessed = true;
        for (std::string h : inv) {
            if (!_blocktree.ContainBlock(h)) {
                std::cout << "sending POS protocol getdata for" << h << "\n";

                std::shared_ptr<MessageObject> ptrToObj = std::make_shared<POSBlockGossipGetData>(h);
                std::shared_ptr<Message> message = std::make_shared<Message>(msg->GetSource(), "POSBLOCK-GETDATA", ptrToObj);
                BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(msg->GetSource(), message);

                invAllProcessed = false;
                break; // request only a single block
            }
        }

        if (invAllProcessed) {
            std::cout << "stop processing inv" << "\n";
            StopProcessingInv();

            // relay POSBLOCK-INV message??
            std::shared_ptr<MessageObject> ptrToObj = std::make_shared<POSBlockGossipInventory>(inv);
            std::vector<PeerId> neighborIds = BL_PeerConnectivityLayer_API::Instance()->GetNeighborPeerIds();
            for (auto neighborId : neighborIds) {
                if (neighborId.GetId() != msg->GetSource().GetId()) {
                    std::cout << "send(relay) inv to " << neighborId.GetId() << "\n";
                    std::shared_ptr<Message> message = std::make_shared<Message>(neighborId, "POSBLOCK-INV", ptrToObj);
                    BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(neighborId, message);
                }
            }
        }
    }
}

void BL_ProtocolLayerPoS::SwitchAsyncEventHandler(AsyncEvent& event) {
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
            std::shared_ptr<POSBlock> minedBlk = event.GetData().GetMinedPOSBlock();
            std::cout << "blockhash:" << minedBlk->GetBlockHash() << "\n";
            std::cout << "blockhash(str):" << minedBlk->GetBlockHash().str() << "\n";
            std::cout << "blockhash:" << libBLEEP::UINT256_t((const unsigned char*)minedBlk->GetBlockHash().str().c_str(), 32) << "\n";
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
            std::shared_ptr<MessageObject> ptrToObj = std::make_shared<POSBlockGossipInventory>(hashes);

            std::vector<PeerId> neighborIds = BL_PeerConnectivityLayer_API::Instance()->GetNeighborPeerIds();
            for (auto &neighborId : neighborIds) {
                std::cout << "send POSBlockinv to " << neighborId.GetId() << "\n";
                std::shared_ptr<Message> message = std::make_shared<Message>(neighborId, "POSBLOCK-INV", ptrToObj);
                BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(neighborId, message);
            }

            break;
        }
    default:
        break;
    }
}

unsigned long BL_ProtocolLayerPoS::random_selection(unsigned long slot_id) {
    if (slot_id == 0) {
        return 0;
    }
    unsigned long random_num = slot_id;
    std::default_random_engine generator(random_num);
    std::uniform_real_distribution<double> distribution(0.0,1.0);
    double number = distribution(generator);

    if (number <= 0)
        return stakes.first();  // TODO: stakes
    if (number >= 1)
        return stakes.last();
    return stakes.pickLeader((unsigned long)(number * stakes.getTotal()));
}
std::shared_ptr<POSBlock> BL_ProtocolLayerPoS::makeBlockTemplate(unsigned long slot_id) {
    std::list<std::shared_ptr<SimpleTransaction>> txs = _txPool->GetTxs(maxTxPerBlock);
    std::shared_ptr<POSBlock> templateBlock = std::make_shared<POSBlock>("", txs);
    templateBlock->SetBlockIdx(_blocktree.GetNextBlockIdx());
    templateBlock->SetPrevBlockHash(_blocktree.GetLastHash());
    return templateBlock;
}
void BL_ProtocolLayerPoS::_slottimerCallback(ev::timer &w, int revents) {
    unsigned long _current_slot = (int)(GetGlobalClock() / slot_interval);
    if (_current_slot == 0) {
        // ignore genesis case
        return;
    }
    if (random_selection((unsigned long)_current_slot) == _creatorNodeId) {
        std::shared_ptr<POSBlock> blk = makeBlockTemplate(_current_slot);
        _posMiner.AsyncMakeBlock(blk);
    }
}
void BL_ProtocolLayerPoS::_startPeriodicSlotStart(double interval) {
    _slottimer.set<BL_ProtocolLayerPoS, &BL_ProtocolLayerPoS::_slottimerCallback>(this);
    _slottimer.set(0, interval);
    _slottimer.start();
}

bool BL_ProtocolLayerPoS::InitiateProtocol() {
    if (!_initiated) {
        std::cout << "initiating ProtocolPoS" << "\n";
        _startPeriodicTxGen(txGenStartAt, txGenInterval);
        _startPeriodicSlotStart(slot_interval);
        _initiated = true;
        stakes.load(stakeDatafile);
        return true;
    } else {
        std::cout << "already initiated protocol" << "\n";
        return false;
    }
}

bool BL_ProtocolLayerPoS::InitiateProtocol(ProtocolParameter* params) {
    POSProtocolParameter* posparams = dynamic_cast<POSProtocolParameter*>(params);
    assert(posparams != nullptr);
    if (!_initiated) {
        std::cout << "initiating ProtocolPoS with custom params" << "\n";
        _startPeriodicTxGen(posparams->txGenStartAt, posparams->txGenInterval);
        _startPeriodicSlotStart(posparams->slot_interval);
        _initiated = true;
        _creatorNodeId = posparams->creatorNodeId;
        stakeDatafile = posparams->stakeDatafile;
        stakes.load(stakeDatafile);
        return true;
    } else {
        std::cout << "already initiated protocol" << "\n";
        return false;
    }
}

bool BL_ProtocolLayerPoS::StopProtocol() {
    // do nothing?
    return true;
}
