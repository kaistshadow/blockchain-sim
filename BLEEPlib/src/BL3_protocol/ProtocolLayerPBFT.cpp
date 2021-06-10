#include "ProtocolLayerPBFT.h"
#include "PBFTProtocolParameter.h"
#include "PBFTBlockGossipProtocolMsg.h"

#include "shadow_memshare_interface.h"

using namespace libBLEEP_BL;


BL_ProtocolLayerPBFT::BL_ProtocolLayerPBFT() : BL_ProtocolLayer_API(),
                                             _txGossipProtocol(_txPool) {
}

void BL_ProtocolLayerPBFT::RecvMsgHandler(std::shared_ptr<Message> msg) {
    std::cout << "recv protocol msg" << "\n";
    MessageType msgType = msg->GetType();
    if (msgType == "TXGOSSIP-INV") {
        _txGossipProtocol.RecvInventoryHandler(msg);
    } else if (msgType == "TXGOSSIP-GETDATA") {
        _txGossipProtocol.RecvGetdataHandler(msg);
    } else if (msgType == "TXGOSSIP-TXS") {
        _txGossipProtocol.RecvTxsHandler(msg);
    } else if (msgType == "PBFTBLOCK-INV") {
        _RecvPBFTBlockInvHandler(msg);
    } else if (msgType == "PBFTBLOCK-GETBLOCKS") {
        _RecvPBFTBlockGetBlocksHandler(msg);
    } else if (msgType == "PBFTBLOCK-GETDATA") {
        _RecvPBFTBlockGetDataHandler(msg);
    } else if (msgType == "PBFTBLOCK-BLK") {
        _RecvPBFTBlockBlkHandler(msg);
    } else if (msgType == "PBFT-PREPREPARE") {
        _RecvPBFTPreprepareHandler(msg);
    } else if (msgType == "PBFT-PREPARE") {
        _RecvPBFTPrepareHandler(msg);
    } else if (msgType == "PBFT-COMMIT") {
        _RecvPBFTCommitHandler(msg);
    } else if (msgType == "PBFT-CHECKPOINT") {
        _RecvPBFTCheckpointHandler(msg);
    } else if (msgType == "PBFT-VIEWCHANGE") {
        _RecvPBFTViewChangeHandler(msg);
    }
}

// TODO: considering the case that prepare from other replica arrives earlier than the preprepare from the primary.
void BL_ProtocolLayerPBFT::_RecvPBFTPreprepareHandler(std::shared_ptr<Message> msg) {
    if (_p == _consensusId) {
        // why primary node get preprepare message? ignore it.
        return;
    }

    std::shared_ptr<PBFTPreprepare> ppr = std::static_pointer_cast<PBFTPreprepare>(msg->GetObject());

    // signature verification    
    std::ostringstream oss;
    unsigned int v = ppr->m->v;
    unsigned int n = ppr->m->n;
    std::string d = ppr->m->str();

    if (v != _v) {
        // view mismatch
        return;
    }
    oss << "PREPREPARE" << v << n << d;
    if (!pubkey[_p].verify(oss.str(), ppr->sign)) {
        // pubkey mismatch. ignore the message.
        return;
    }
    if (n <= _h || n >= _h + _k) {
        // sequence range mismatch
        return;
    }
    // TODO: check message validity (block is well connected to the last block in the tree)


    _preprepareMsgsLock.lock();
    if (_preprepareMsgs.has(v, n)) {
        _preprepareMsgsLock.unlock();
        return;
    }

    // setup target
    _current_consensus.set(v, n, d);
    // clear n enable prepare reception
    _current_prepare.reset();
    // send prepare message to everyone in the consensus
    for (auto consensusNeighbor : consensusNeighbors) {
        std::shared_ptr<MessageObject> ptrToObj = std::make_shared<PBFTPrepare>(v, n, d, _consensusId);
        std::shared_ptr<Message> message = std::make_shared<Message>(consensusNeighbor, "PBFT-PREPARE", ptrToObj);
        BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(consensusNeighbor, message);
    }

}
void BL_ProtocolLayerPBFT::_RecvPBFTPrepareHandler(std::shared_ptr<Message> msg) {
    if (_current_prepare.isDisabled()) {
        return;
    }
    std::shared_ptr<PBFTPrepare> pr = std::static_pointer_cast<PBFTPrepare>(msg->GetObject());

    // signature verification    
    std::ostringstream oss;
    unsigned int v = _current_consensus.v;
    unsigned int n = _current_consensus.n;
    std::string d = _current_consensus.d;
    unsigned int i = msg->GetSource();
    oss << "PREPARE" << v << n << d << i;
    if (!pubkey[i].verify(oss.str(), pr->sign)) {
        // pubkey mismatch. ignore the message.
        return;
    }

    // try save
    if (_current_prepare.hasNeighbor(i)) {
        // already know
        return;
    }
    _current_prepare.addNeighbor(i);

    if (_current_prepare.NeighborCount() == 2 * _f) {
        // next phase enabled, so stop receiving prepare message
        _current_prepare.disable();
        // clear n enable commit reception
        _current_commit.reset();
        // send commit message to everyone in the consensus
        for (auto consensusNeighbor : consensusNeighbors) {
            std::shared_ptr<MessageObject> ptrToObj = std::make_shared<PBFTCommit>(v, n, d, _consensusId);
            std::shared_ptr<Message> message = std::make_shared<Message>(consensusNeighbor, "PBFT-COMMIT", ptrToObj);
            BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(consensusNeighbor, message);
        }
    }
}
void BL_ProtocolLayerPBFT::_RecvPBFTCommitHandler(std::shared_ptr<Message> msg) {
    if (_current_commit.isDisabled()) {
        return;
    }
    std::shared_ptr<PBFTCommit> cm = std::static_pointer_cast<PBFTCommit>(msg->GetObject());

    // signature verification    
    std::ostringstream oss;
    unsigned int v = _current_consensus.v;
    unsigned int n = _current_consensus.n;
    std::string d = _current_consensus.d;
    unsigned int i = msg->GetSource();
    oss << "COMMIT" << v << n << d << i;
    if (!pubkey[i].verify(oss.str(), cm->sign)) {
        // pubkey mismatch. ignore the message.
        return;
    }

    // try save
    if (_current_commit.hasNeighbor(i)) {
        // already know
        return;
    }
    _current_commit.addNeighbor(i);

    if (_current_commit.NeighborCount() == 2 * _f) {
        // next phase enabled, so stop receiving commit message
        _current_commit.disable();

        // TODO: phase end.
    }
}


void BL_ProtocolLayerPBFT::_RecvPBFTBlockInvHandler(std::shared_ptr<Message> msg) {
    std::cout << "recv PBFT protocol msg (PBFTBLOCK-INV)" << "\n";

    std::shared_ptr<PBFTBlockGossipInventory> inv = std::static_pointer_cast<PBFTBlockGossipInventory>(msg->GetObject());
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

        std::shared_ptr<MessageObject> ptrToObj = std::make_shared<PBFTBlockGossipGetBlocks>(blockLocator);
        std::shared_ptr<Message> message = std::make_shared<Message>(msg->GetSource(), "PBFTBLOCK-GETBLOCKS", ptrToObj);
        BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(msg->GetSource(), message);
    } else {
        // Register processing inventory
        SetProcessingInv(hashes);
        StartProcessingInv();

        // Second, if block tree lacks of actual block, request the block
        bool containAll = true;
        for (std::string h : hashes) {
            if (!_blocktree.ContainBlock(h)) {
                std::cout << "send PBFT getdata message" << "\n";

                std::shared_ptr<MessageObject> ptrToObj = std::make_shared<PBFTBlockGossipGetData>(h);
                std::shared_ptr<Message> message = std::make_shared<Message>(msg->GetSource(), "PBFTBLOCK-GETDATA", ptrToObj);
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

void BL_ProtocolLayerPBFT::_RecvPBFTBlockGetBlocksHandler(std::shared_ptr<Message> msg) {
    std::cout << "received PBFT protocol getblocks message" << "\n";

    std::shared_ptr<PBFTBlockGossipGetBlocks> getblocks = std::static_pointer_cast<PBFTBlockGossipGetBlocks>(msg->GetObject());
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

    // create PBFTBLOCK-INV message
    std::shared_ptr<MessageObject> ptrToObj = std::make_shared<PBFTBlockGossipInventory>(inv);
    std::shared_ptr<Message> message = std::make_shared<Message>(msg->GetSource(), "PBFTBLOCK-INV", ptrToObj);
    BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(msg->GetSource(), message);
}

void BL_ProtocolLayerPBFT::_RecvPBFTBlockGetDataHandler(std::shared_ptr<Message> msg) {

    std::cout << "received PBFT protocol getdata message" << "\n";

    std::shared_ptr<PBFTBlockGossipGetData> getdata = std::static_pointer_cast<PBFTBlockGossipGetData>(msg->GetObject());
    std::string blockhash = getdata->GetBlockHash();
    std::cout << "getdata's block hash:" << blockhash << "\n";

    M_Assert(_blocktree.ContainBlockHash(blockhash), "hash must be synchronized already.");
    M_Assert(_blocktree.ContainBlock(blockhash), "must have blockdata.");

    // create PBFTBLOCK-BLK message
    std::shared_ptr<MessageObject> ptrToObj = std::make_shared<PBFTBlockGossipBlk>(_blocktree.GetBlock(blockhash));
    std::shared_ptr<Message> message = std::make_shared<Message>(msg->GetSource(), "PBFTBLOCK-BLK", ptrToObj);
    BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(msg->GetSource(), message);
}


void BL_ProtocolLayerPBFT::_RecvPBFTBlockBlkHandler(std::shared_ptr<Message> msg) {
    std::cout << "received PBFT protocol block message" << "\n";
}

void BL_ProtocolLayerPBFT::SwitchAsyncEventHandler(AsyncEvent& event) {
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
            std::shared_ptr<PBFTBlock> minedBlk = event.GetData().GetMinedBlock();
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
            std::shared_ptr<MessageObject> ptrToObj = std::make_shared<PBFTBlockGossipInventory>(hashes);

            std::vector<PeerId> neighborIds = BL_PeerConnectivityLayer_API::Instance()->GetNeighborPeerIds();
            for (auto &neighborId : neighborIds) {
                std::cout << "send PBFTBlockinv to " << neighborId.GetId() << "\n";
                std::shared_ptr<Message> message = std::make_shared<Message>(neighborId, "PBFTBLOCK-INV", ptrToObj);
                BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(neighborId, message);
            }

            break;
        }
    default:
        break;
    }
}

bool BL_ProtocolLayerPBFT::InitiateProtocol() {
    if (!_initiated) {
        std::cout << "initiating ProtocolPBFT" << "\n";
        _startPeriodicTxGen(txGenStartAt, txGenInterval);
        _initiated = true;

        return true;
    } else {
        std::cout << "already initiated protocol" << "\n";
        return false;
    }
}

bool BL_ProtocolLayerPBFT::InitiateProtocol(ProtocolParameter* params) {
//    PBFTProtocolParameter& pbftparams = dynamic_cast<PBFTProtocolParameter &>(params);
    PBFTProtocolParameter* pbftparams = dynamic_cast<PBFTProtocolParameter*>(params);
    assert(pbftparams != nullptr);
    if (!_initiated) {
        std::cout << "initiating ProtocolPBFT with custom params" << "\n";
        _startPeriodicTxGen(pbftparams->txGenStartAt, pbftparams->txGenInterval);
        _initiated = true;

        miningtime = pbftparams->miningtime;
        miningnodecnt = pbftparams->miningnodecnt;
        std::cout << "miningtime:" << miningtime << "\n";
        std::cout << "miningnodecnt:" << miningnodecnt << "\n";

        return true;
    } else {
        std::cout << "already initiated protocol" << "\n";
        return false;
    }
}

bool BL_ProtocolLayerPBFT::StopProtocol() {
    // do nothing?
    return true;
}
