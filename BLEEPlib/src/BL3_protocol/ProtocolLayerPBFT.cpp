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
    } else if (msgType == "PBFT-PREPREPARE") {
        _RecvPBFTPreprepareHandler(msg);
    } else if (msgType == "PBFT-PREPARE") {
        _RecvPBFTPrepareHandler(msg);
    } else if (msgType == "PBFT-COMMIT") {
        _RecvPBFTCommitHandler(msg);
//    } else if (msgType == "PBFT-CHECKPOINT") {
//        _RecvPBFTCheckpointHandler(msg);
//    } else if (msgType == "PBFT-VIEWCHANGE") {
//        _RecvPBFTViewChangeHandler(msg);
    }
}

void BL_ProtocolLayerPBFT::_joinTimerCallback(ev::timer &w, int revents) {
    std::cout << "Debug _joinTimerCallback called\n";
    // if all consensus node is connected, stop timer.
    if (_config.isAllConnected()) {
        std::cout << "Debug _joinTimerCallback::all connected\n";
        w.stop();
        // initialize current view as 0, sequence as 0
        _v = 0;
        _n = 0;
        // pick a _config member as first primary
        _p = _v % _config.size();
        // setup faulty limit
        if (_config.size() < 4) {
            // cannot make proper PBFT algorithm
            return;
        }
        _f = (_config.size() - 1) / 3;
        _msgs.setFaultyLimit(_f);
        // setup checkout boundary
        _h = 0;
        // setup viewchange timer
        _initViewChangeStarter();
        // if my id is primary, call _StartPreprepare
        std::cout << "Debug _joinTimerCallback:: _p=" << _p << ", _consensusNodeID=" << _consensusNodeID << "\n";
        if (_p == _consensusNodeID) {
            _StartPreprepare();
        }
        return;
    }

    // for all connection, check below
    // if the connection is already mapped with pubkey, continue.
    // else send joinRequest
    std::vector<PeerId> neighborIds = BL_PeerConnectivityLayer_API::Instance()->GetNeighborPeerIds();
    for (auto neighborId : neighborIds) {
        std::cout << "Debug join request target:" << neighborId.GetId() << "\n";
        unsigned long pk;
        if (_config.getPeerPubkey(neighborId.GetId(), pk) == 0) {
            std::cout << "Debug already set neighbor\n";
            continue;
        }
        std::shared_ptr<MessageObject> ptrToObj = std::make_shared<PBFTJoinRequest>();
        std::shared_ptr<Message> message = std::make_shared<Message>(neighborId, "PBFT-JOINREQ", ptrToObj);
        BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(neighborId, message);
    }
}



std::shared_ptr<PBFTBlock> BL_ProtocolLayerPBFT::makeBlockTemplate() {
    std::list<std::shared_ptr<SimpleTransaction>> txs = _txPool->GetTxs(maxTxPerBlock); // TODO: change txpool logic
    std::shared_ptr<PBFTBlock> templateBlock = std::make_shared<PBFTBlock>("", txs);
    templateBlock->SetPrevBlockHash(_blocktree.GetLastHash());
    templateBlock->SetBlockIdx(_blocktree.GetNextBlockIdx());
    return templateBlock;
}

void BL_ProtocolLayerPBFT::_RecvPBFTJoinRequestHandler(std::shared_ptr<Message> msg) {
    std::cout << "Debug PBFTJoinRequest message received\n";
    std::shared_ptr<PBFTJoinRequest> jreq = std::static_pointer_cast<PBFTJoinRequest>(msg->GetObject());

    std::ostringstream oss;
    oss << "JOINREQ" << _consensusNodeID;
    std::string signText = oss.str();
    std::string sign = PBFTSignature(_secret, signText);
    std::cout << "Debug PBFTJoinRequest content: " << _consensusNodeID << ", " << sign << "\n";

    std::shared_ptr<MessageObject> ptrToObj = std::make_shared<PBFTJoinResponse>(_consensusNodeID, sign);
    std::shared_ptr<Message> message = std::make_shared<Message>(msg->GetSource(), "PBFT-JOINRES", ptrToObj);
    BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(msg->GetSource(), message);
}
void BL_ProtocolLayerPBFT::_RecvPBFTJoinResponseHandler(std::shared_ptr<Message> msg) {
    std::cout << "Debug PBFTJoinResponse message received\n";
    std::shared_ptr<PBFTJoinResponse> jres = std::static_pointer_cast<PBFTJoinResponse>(msg->GetObject());

    std::ostringstream oss;
    oss << "JOINREQ" << jres->consensusNodeId;

    PBFTPubkey pk;
    std::cout << "Debug join response:" << jres->consensusNodeId << "-"<< msg->GetSource().GetId() << ", " << jres->sign << "\n";
    pk.setID(jres->consensusNodeId);
    if (!PBFTVerify(pk, jres->sign, oss.str())) {
        return;
    }
    _config.assignSource(jres->consensusNodeId, msg->GetSource().GetId());
}

void BL_ProtocolLayerPBFT::_StartPreprepare() {
    std::cout << "Debug StartPreprepare called\n";
   if (_p != _consensusNodeID) {
       // why non-primary node create preprepare message? ignore it.
       return;
   }

   std::shared_ptr<PBFTBlock> blk = makeBlockTemplate();
   blk->CalcHash();

   // make preprepare message
   std::ostringstream oss;
   unsigned int n = _n++;
   std::string d = PBFTDigest(blk->GetBlockHash().str());  // simply get digest with block hash, for just simple abstraction PBFT logic.
   oss << "PREPREPARE" << _v << n << d;
   std::string signText = oss.str();
   std::shared_ptr<MessageObject> ptrToObj = std::make_shared<PBFTPreprepare>(_v, n, d, PBFTSignature(_secret, signText), blk);

    std::cout<< "Request block hash = "<<blk->GetBlockHash()<<" / v: "<<_v<<" / n: "<<n<<"\n";

   // send preprepare message to everyone in the consensus
   for (auto consensusNeighbor : _config.getConsensusMap()) {
       std::shared_ptr<Message> message = std::make_shared<Message>(consensusNeighbor.second, "PBFT-PREPREPARE", ptrToObj);
       BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(consensusNeighbor.second, message);
   }
   _msgs.addPreprepared(_v, n, blk);
}

void BL_ProtocolLayerPBFT::_RecvPBFTPreprepareHandler(std::shared_ptr<Message> msg) {
    std::cout << "Debug preprepare received\n";
    if (_p == _consensusNodeID) {
        // why primary node get preprepare message? ignore it.
        return;
    }

    std::shared_ptr<PBFTPreprepare> ppr = std::static_pointer_cast<PBFTPreprepare>(msg->GetObject());

    std::ostringstream oss;
    unsigned int v = ppr->view;
    unsigned int n = ppr->sequence;
    std::string d = ppr->digest;
    std::shared_ptr<PBFTBlock> m = ppr->blk;
    /* verification in PBFT paper 4.2 Normal-Case Operation */
    // the signatures in the pre-prepare message are correct and d is the digest for m
    oss << "PREPREPARE" << v << n << d;
    unsigned long pubkeyId;
    if (_config.getPeerPubkey(msg->GetSource().GetId(), pubkeyId) == -1) {
        // cannot find corresponding pubkey
        std::cout << "Debug _RecvPBFTPreprepareHandler::cannot find corresponding pubkey\n";
        return;
    }
    PBFTPubkey pk;
    pk.setID(pubkeyId);
    if (!PBFTVerify(pk, ppr->sign, oss.str())) {
        // signature mismatch
        std::cout << "Debug _RecvPBFTPreprepareHandler::signature mismatch\n";
        return;
    }
    if (d != PBFTDigest(m->GetBlockHash().str())) {
        // digest for m mismatch
        std::cout << "Debug _RecvPBFTPreprepareHandler::digest for m mismatch\n";
        return;
    }
    // it is in view v
    if (v != _v) {
        // view mismatch
        std::cout << "Debug _RecvPBFTPreprepareHandler::view mismatch\n";
        return;
    }
    // it has not accepted a pre-prepare message for view v and sequence number n containing a different digest
    if (_msgs.hasPreprepared(v, n)) {
        // already received message with same v, n
        std::cout << "Debug _RecvPBFTPreprepareHandler::already received message with same v, n\n";
        return;
    }
    // the sequence number in the pre-prepare message is between a low water mark h, and a high water mark H
    if (n < _h || n >= _h + _k) {
        // sequence range mismatch
        std::cout << "Debug _RecvPBFTPreprepareHandler::sequence range mismatch\n";
        return;
    }

    // check message validity (block is well connected to the last block in the tree)
    if (m->GetPrevBlockHash() != _blocktree.GetLastHash()) {
        // last tip mismatch
        std::cout << "Debug received block's parent does not match\n";
        // TODO: add followup protocol
        return;
    }

// unused codes
//    // setup target
//    _current_consensus.set(v, n, d);
//    // clear n enable prepare reception
//    _current_prepare.reset();

    // enter prepare phase
    // make prepare message
    std::ostringstream oss2;
    unsigned long i = _consensusNodeID;
    oss2 << "PREPARE" << v << n << d << i;
    std::string signText = oss2.str();
    std::shared_ptr<MessageObject> ptrToObj = std::make_shared<PBFTPrepare>(_v, n, d, i, PBFTSignature(_secret, signText));
    // send prepare message to everyone in the consensus
    for (auto consensusNeighbor : _config.getConsensusMap()) {
        std::shared_ptr<Message> message = std::make_shared<Message>(consensusNeighbor.second, "PBFT-PREPARE", ptrToObj);
        BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(consensusNeighbor.second, message);
    }

    _msgs.addPreprepared(v, n, m);
    if (_msgs.predPreparedOnce(v, n)) {
        _Commit(v, n, d, _consensusNodeID);
    }
}
void BL_ProtocolLayerPBFT::_RecvPBFTPrepareHandler(std::shared_ptr<Message> msg) {
    std::cout << "Debug prepare received\n";
    std::shared_ptr<PBFTPrepare> pr = std::static_pointer_cast<PBFTPrepare>(msg->GetObject());

    // signature verification
    std::ostringstream oss;
    unsigned int v = pr->view;
    unsigned int n = pr->sequence;
    std::string d = pr->digest;
    unsigned int i = pr->memberID;
    oss << "PREPARE" << v << n << d << i;
    PBFTPubkey pk;
    pk.setID(i);
    if (!PBFTVerify(pk, pr->sign, oss.str())) {
        // signature mismatch
        std::cout << "Debug _RecvPBFTPrepareHandler::signature mismatch\n";
        std::cout << "Debug sign: " << pr->sign << ", answer: " << oss.str() << "\n";
        return;
    }

    _msgs.addPrepared(v, n, d, i);
    if (_msgs.predPreparedOnce(v, n)) {
        _Commit(v, n, d, _consensusNodeID);
    }
}
void BL_ProtocolLayerPBFT::_Commit(unsigned long v, unsigned int n, std::string d, unsigned long i) {
    std::cout << "Debug commit called for " << v << ", " << n << ", " << d << ", " << i << "\n";
    std::ostringstream oss;
    oss << "COMMIT" << v << n << d << i;
    std::string signText = oss.str();
    std::shared_ptr<MessageObject> ptrToObj = std::make_shared<PBFTCommit>(v, n, d, i, PBFTSignature(_secret, signText));
    // send prepare message to everyone in the consensus
    for (auto consensusNeighbor : _config.getConsensusMap()) {
        std::shared_ptr<Message> message = std::make_shared<Message>(consensusNeighbor.second, "PBFT-COMMIT", ptrToObj);
        BL_PeerConnectivityLayer_API::Instance()->SendMsgToPeer(consensusNeighbor.second, message);
    }
    _msgs.addCommit(v, n, d, i);
    if (_msgs.predCommittedLocalOnce(v, n)) {
        std::cout << "Debug commit local true v: "<<v<<" / n: "<<n<<"\n";
        // add block to chain
        auto blkptr = _msgs.getMessage(v, n);
        _blocktree.AppendBlock(blkptr);
        // if my id is primary id, start next consensus
        if (_p == _consensusNodeID) {
            _StartPreprepare();
        }
    }
}
void BL_ProtocolLayerPBFT::_RecvPBFTCommitHandler(std::shared_ptr<Message> msg) {
    std::cout << "Debug commit received\n";
    std::shared_ptr<PBFTCommit> cm = std::static_pointer_cast<PBFTCommit>(msg->GetObject());

    std::ostringstream oss;
    unsigned long v = cm->view;
    unsigned int n = cm->sequence;
    std::string d = cm->digest;
    unsigned long i = cm->memberID;
    oss << "COMMIT" << v << n << d << i;
    PBFTPubkey pk;
    pk.setID(i);
    if (!PBFTVerify(pk, cm->sign, oss.str())) {
        // pubkey mismatch. ignore the message.
        return;
    }
    // it is in view v
    if (v != _v) {
        // view mismatch
        return;
    }
    // the sequence number in the pre-prepare message is between a low water mark h, and a high water mark H
    if (n < _h || n >= _h + _k) {
        // sequence range mismatch
        return;
    }

    _msgs.addCommit(v, n, d, i);
    if (_msgs.predCommittedLocalOnce(v, n)) {
        std::cout << "Debug commit local true v: "<<v<<" / n: "<<n<<"\n";
        // add block to chain
        auto blkptr = _msgs.getMessage(v, n);
        _blocktree.AppendBlock(blkptr);
        // if my id is primary id, start next consensus
        if (_p == _consensusNodeID) {
            _StartPreprepare();
        }
    }
}

void BL_ProtocolLayerPBFT::_changeViewCallback(ev::timer &w, int revents) {

}
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
//    if (!PBFTVerify(pubkey[i], cm->sign, oss.str())) {
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
//    if (!PBFTVerify(pubkey[i], cm->sign, oss.str())) {
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

        std::cout << "Debug _consensusNodeID:" << _consensusNodeID << "\n";
        _secret.setID(_consensusNodeID);
        _pubkey.setID(_consensusNodeID);

        _config.load("config.txt", _consensusNodeID);

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

        _consensusNodeID = pbftparams->consensusNodeID;
        std::cout << "Debug _consensusNodeID:" << _consensusNodeID << "\n";
        _secret.setID(_consensusNodeID);
        _pubkey.setID(_consensusNodeID);

        _config.load(configFile, _consensusNodeID);

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