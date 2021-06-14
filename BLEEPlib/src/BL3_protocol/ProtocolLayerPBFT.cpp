#include "ProtocolLayerPBFT.h"
#include "PBFTProtocolParameter.h"
#include "PBFTConsensusProtocolMsg.h"
#include "PBFTSigning.h"

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
    } else if (msgType == "PBFT-JOINREQ") {
        _RecvPBFTJoinRequestHandler(msg);
    } else if (msgType == "PBFT-JOINRES") {
        _RecvPBFTJoinResponseHandler(msg);
//    } else if (msgType == "PBFT-PREPREPARE") {
//        _RecvPBFTPreprepareHandler(msg);
//    } else if (msgType == "PBFT-PREPARE") {
//        _RecvPBFTPrepareHandler(msg);
//    } else if (msgType == "PBFT-COMMIT") {
//        _RecvPBFTCommitHandler(msg);
//    } else if (msgType == "PBFT-CHECKPOINT") {
//        _RecvPBFTCheckpointHandler(msg);
//    } else if (msgType == "PBFT-VIEWCHANGE") {
//        _RecvPBFTViewChangeHandler(msg);
    }
}

void BL_ProtocolLayerPBFT::_joinTimerCallback(ev::timer &w, int revents) {
    // if all consensus node is connected, stop timer.
    if (_config.isAllConnected()) {
        w.stop();
        return;
    }

    // for all connection, check below
    // if the connection is already mapped with pubkey, continue.
    // else send joinRequest
    std::vector<PeerId> neighborIds = BL_PeerConnectivityLayer_API::Instance()->GetNeighborPeerIds();
    for (auto neighborId : neighborIds) {
        unsigned long pk;
        if (_config.getPeerPubkey(neighborId.GetId(), pk) == 0) {
            continue;
        }
        std::shared_ptr<MessageObject> ptrToObj = std::make_shared<PBFTJoinRequest>();
        std::shared_ptr<Message> message = std::make_shared<Message>(neighborId, "PBFT-JOINREQ", ptrToObj);
        BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(neighborId, message);
    }
}

std::string _digest(std::string plainText) {
    return plainText;
}

std::string _PBFTSignature(PBFTSecret k, std::string plainText) {
    std::string d = _digest(plainText);
    return k.sign(d);
}
bool _PBFTVerify(PBFTPubkey p, std::string sig, std::string plainText) {
    std::string d = _digest(plainText);
    return p.verify(sig, d);
}

void BL_ProtocolLayerPBFT::_RecvPBFTJoinRequestHandler(std::shared_ptr<Message> msg) {
    std::shared_ptr<PBFTJoinRequest> jreq = std::static_pointer_cast<PBFTJoinRequest>(msg->GetObject());

    std::ostringstream oss;
    oss << "JOINREQ" << _consensusNodeID;
    std::string signText = oss.str();
    std::string sign = _PBFTSignature(_secret, signText);

    std::shared_ptr<MessageObject> ptrToObj = std::make_shared<PBFTJoinResponse>(_consensusNodeID, sign);
    std::shared_ptr<Message> message = std::make_shared<Message>(msg->GetSource(), "PBFT-JOINRES", ptrToObj);
    BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(msg->GetSource(), message);
}
void BL_ProtocolLayerPBFT::_RecvPBFTJoinResponseHandler(std::shared_ptr<Message> msg) {
    std::shared_ptr<PBFTJoinResponse> jres = std::static_pointer_cast<PBFTJoinResponse>(msg->GetObject());

    std::ostringstream oss;
    oss << "JOINREQ" << jres->consensusNodeId;

    PBFTPubkey pk;
    pk.setID(jres->consensusNodeId);
    if (!pk.verify(oss.str(), jres->sign)) {
        return;
    }
    _config.assignSource(jres->consensusNodeId, msg->GetSource().GetId());
}

//void BL_ProtocolLayerPBFT::_StartPreprepare() {
//    if (_p != _consensusNodeID) {
//        // why non-primary node create preprepare message? ignore it.
//        return;
//    }
//
//    std::shared_ptr<PBFTBlock> blk = _makeTemplate();
//
//    // make preprepare message
//    std::ostringstream oss;
//    unsigned int n = _n++;
//    std::string d = _digest(blk->str());
//    oss << "PREPREPARE" << _v << n << d;
//    std::string signText = oss.str();
//    std::shared_ptr<MessageObject> ptrToObj = std::make_shared<PBFTPreprepare>(_v, n, d, _PBFTSignature(_key, signText), blk);
//
//    // send preprepare message to everyone in the consensus
//    for (auto consensusNeighbor : consensusNeighbors) {
//        std::shared_ptr<Message> message = std::make_shared<Message>(consensusNeighbor, "PBFT-PREPREPARE", ptrToObj);
//        BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(consensusNeighbor, message);
//    }
//}
//
//// TODO: considering the case that prepare from other replica arrives earlier than the preprepare from the primary.
//void BL_ProtocolLayerPBFT::_RecvPBFTPreprepareHandler(std::shared_ptr<Message> msg) {
//    if (_p == _consensusNodeID) {
//        // why primary node get preprepare message? ignore it.
//        return;
//    }
//
//    std::shared_ptr<PBFTPreprepare> ppr = std::static_pointer_cast<PBFTPreprepare>(msg->GetObject());
//
//    // verification
//    std::ostringstream oss;
//    unsigned int v = ppr->v;
//    unsigned int n = ppr->n;
//    std::string d = ppr->d;
//    if (v != _v) {
//        // view mismatch
//        return;
//    }
//    if (d != digest(ppr->m)) {
//        // digest for m mismatch
//        return;
//    }
//    oss << "PREPREPARE" << v << n << d;
//    if (!pubkey[_p].verify(oss.str(), ppr->sign)) {
//        // signature mismatch
//        return;
//    }
//    if (n <= _h || n >= _h + _k) {
//        // sequence range mismatch
//        return;
//    }
//    // TODO: check message validity (block is well connected to the last block in the tree)
//
//    if (_preprepareMsgs.has(v, n)) {
//        // already received message with same v, n
//        return;
//    }
//
//    // setup target
//    _current_consensus.set(v, n, d);
//    // clear n enable prepare reception
//    _current_prepare.reset();
//    // send prepare message to everyone in the consensus
//    for (auto consensusNeighbor : consensusNeighbors) {
//        std::shared_ptr<MessageObject> ptrToObj = std::make_shared<PBFTPrepare>(v, n, d, _consensusNodeID);
//        std::shared_ptr<Message> message = std::make_shared<Message>(consensusNeighbor, "PBFT-PREPARE", ptrToObj);
//        BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(consensusNeighbor, message);
//    }
//
//}
//void BL_ProtocolLayerPBFT::_RecvPBFTPrepareHandler(std::shared_ptr<Message> msg) {
//    if (_current_prepare.isDisabled()) {
//        return;
//    }
//    std::shared_ptr<PBFTPrepare> pr = std::static_pointer_cast<PBFTPrepare>(msg->GetObject());
//
//    // signature verification
//    std::ostringstream oss;
//    unsigned int v = _current_consensus.v;
//    unsigned int n = _current_consensus.n;
//    std::string d = _current_consensus.d;
//    unsigned int i = pr->i;
//    oss << "PREPARE" << v << n << d << i;
//    if (!pubkey[i].verify(oss.str(), pr->sign)) {
//        // pubkey mismatch. ignore the message.
//        return;
//    }
//
//    // try save
//    if (_current_prepare.hasNeighbor(i)) {
//        // already know
//        return;
//    }
//    _current_prepare.addNeighbor(i);
//
//    if (_current_prepare.NeighborCount() == 2 * _f) {
//        // next phase enabled, so stop receiving prepare message
//        _current_prepare.disable();
//        // clear n enable commit reception
//        _current_commit.reset();
//        // send commit message to everyone in the consensus
//        for (auto consensusNeighbor : consensusNeighbors) {
//            std::shared_ptr<MessageObject> ptrToObj = std::make_shared<PBFTCommit>(v, n, d, _consensusId);
//            std::shared_ptr<Message> message = std::make_shared<Message>(consensusNeighbor, "PBFT-COMMIT", ptrToObj);
//            BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(consensusNeighbor, message);
//        }
//    }
//}
//void BL_ProtocolLayerPBFT::_RecvPBFTCommitHandler(std::shared_ptr<Message> msg) {
//    if (_current_commit.isDisabled()) {
//        return;
//    }
//    std::shared_ptr<PBFTCommit> cm = std::static_pointer_cast<PBFTCommit>(msg->GetObject());
//
//    // signature verification
//    std::ostringstream oss;
//    unsigned int v = _current_consensus.v;
//    unsigned int n = _current_consensus.n;
//    std::string d = _current_consensus.d;
//    unsigned int i = cm->i;
//    oss << "COMMIT" << v << n << d << i;
//    if (!pubkey[i].verify(oss.str(), cm->sign)) {
//        // pubkey mismatch. ignore the message.
//        return;
//    }
//
//    // try save
//    if (_current_commit.hasNeighbor(i)) {
//        // already know
//        return;
//    }
//    _current_commit.addNeighbor(i);
//
//    if (_current_commit.NeighborCount() == 2 * _f) {
//        // next phase enabled, so stop receiving commit message
//        _current_commit.disable();
//
//        // TODO: phase end.
//    }
//}
//
//// TODO: implement
//void BL_ProtocolLayerPBFT::_RecvPBFTCheckpointHandler(std::shared_ptr<Message> msg) {
//    std::shared_ptr<PBFTCheckpoint> cp = std::static_pointer_cast<PBFTCheckpoint>(msg->GetObject());
//
//    // signature verification
//    std::ostringstream oss;
//    unsigned int n = cp->n;
//    std::string d = cp->d;
//    unsigned int i = cp->i;
//    oss << "CHECKPOINT" << n << d << i;
//    if (!pubkey[i].verify(oss.str(), cm->sign)) {
//        // pubkey mismatch. ignore the message.
//        return;
//    }
//
//    _checkpoints.add(n, d, i);
//    if (_checkpoints.count(n, d) > 2 * _f) {
//        _checkpoints.dropLE(n);
//        _recentStableCheckpoint.set(n, d);
//    }
//
//}
//
//// TODO: implement
//void BL_ProtocolLayerPBFT::_RecvPBFTViewChangeHandler(std::shared_ptr<Message> msg) {
//    std::shared_ptr<PBFTViewChange> vc = std::static_pointer_cast<PBFTViewChange>(msg->GetObject());
//
//    // signature verification
//    std::ostringstream oss;
//    unsigned int v_1 = vc->v_1;
//    unsigned int n = vc->n;
//    Commit C = vc->C;
//    std::list<Commit> P = vc->P;
//    unsigned int i = vc->i;
//
//    std::string n = vc->d;
//    unsigned int i = vc->i;
//    oss << "VIEWCHANGE" <<  << d << i;
//    if (!pubkey[i].verify(oss.str(), cm->sign)) {
//        // pubkey mismatch. ignore the message.
//        return;
//    }
//}

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
        _config.load("config.txt");

        _secret.setID(_consensusNodeID);
        _pubkey.setID(_consensusNodeID);

        _initJoinTimer();

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
        _config.load(configFile);

        _consensusNodeID = pbftparams->consensusNodeID;
        _secret.setID(_consensusNodeID);
        _pubkey.setID(_consensusNodeID);

        _initJoinTimer();

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
