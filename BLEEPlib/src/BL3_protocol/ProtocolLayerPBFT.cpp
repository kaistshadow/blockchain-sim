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

// TODO: implement
void BL_ProtocolLayerPBFT::_RecvPBFTCheckpointHandler(std::shared_ptr<Message> msg) {
}

// TODO: implement
void BL_ProtocolLayerPBFT::_RecvPBFTViewChangeHandler(std::shared_ptr<Message> msg) {
}

void BL_ProtocolLayerPBFT::SwitchAsyncEventHandler(AsyncEvent& event) {
    switch (event.GetType()) {
        case AsyncEventEnum::ProtocolRecvMsg:
        {
            std::shared_ptr<Message> msg = event.GetData().GetProtocolMsg();
            RecvMsgHandler(msg);
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
        txGenStartAt = pbftparams->txGenStartAt;
        txGenInterval = pbftparams->txGenInterval;
        _startPeriodicTxGen(txGenStartAt, txGenInterval);
        _initiated = true;

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
