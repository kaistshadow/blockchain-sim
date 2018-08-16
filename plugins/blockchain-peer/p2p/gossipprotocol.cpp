#include "gossipprotocol.h"
#include "socketmessage.h"
#include "simplepeerlist.h"
#include "../blockchain/txpool.h"
#include "socket.h"

SimpleGossipProtocol* SimpleGossipProtocol::instance = 0;

SimpleGossipProtocol* SimpleGossipProtocol::GetInstance() {
    if (instance == 0) {
        instance = new SimpleGossipProtocol();
    }
    return instance;
}

/**
 * Run gossip protocol.
 * Receive any pending requests from inPeerList, and broadcast them to outPeerList.
 * TODO : Resolve infinite broadcast problem (no duplicate check for message)
 */
void SimpleGossipProtocol::RunGossipProtocol(P2PMessage msg) {
    if (msg.type == P2PMessage_TRANSACTION) {
        PeerList outPeerList = SimplePeerList::GetInstance()->GetOutPeerList();
        for (PeerList::iterator it = outPeerList.begin(); it != outPeerList.end(); it++) {    
            Peer* p = *it;
            if (p->conn_status == CONNECTED) {
                // create SocketMessage and insert into the queue
                SocketMessage socketMsg;
                socketMsg.SetSocketfd(p->sfd);
                socketMsg.SetP2PMessage(msg);
                std::string payload = GetSerializedString(socketMsg);
                socketMsg.SetPayload(payload);

                SocketInterface::GetInstance()->PushToQueue(socketMsg);
            }
        }
        
        TxPool::GetInstance()->PushTxToQueue(msg.tx);
    }
}

void SimpleGossipProtocol::ProcessQueue() {
    while (!msgQueue.empty()) {
        P2PMessage msg = msgQueue.front();
        
        RunGossipProtocol(msg);

        msgQueue.pop();
    }
}
