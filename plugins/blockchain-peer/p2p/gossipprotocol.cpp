#include "gossipprotocol.h"
#include "socketmessage.h"
#include "simplepeerlist.h"
#include "../blockchain/txpool.h"
#include "socket.h"
#include "../consensus/simpleconsensus.h"

#include <boost/variant.hpp>

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
        
        Transaction *tx = boost::get<Transaction>(&msg.data);
        if (tx) {
            TxPool::GetInstance()->PushTxToQueue(*tx);
        }
        else {
            std::cout << "Wrong data in P2PMessage" << "\n";
            exit(1);
        }
    }
    else if (msg.type == P2PMessage_BLOCK) {
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

        Block *blk = boost::get<Block>(&msg.data);
        if (blk) {
            std::cout << "Following block is received" << "\n";
            std::cout << *blk << "\n";
        }
        else {
            std::cout << "Wrong data in P2PMessage" << "\n";
            exit(1);
        }
    }
    else if (msg.type == P2PMessage_SIMPLECONSENSUSMESSAGE) {
        // Do not propagate a simpleconsensusmessage to p2p network.
        // We assume that simpleconsensusmessage are targeted message. (unlike a non-targeted p2p message)

        SimpleConsensusMessage *consensusMsg = boost::get<SimpleConsensusMessage>(&msg.data);
        if (consensusMsg) {
            SimpleConsensus::GetInstance()->PushToQueue(*consensusMsg);            
        } else {
            std::cout << "Wrong data in P2PMessage" << "\n";
            exit(1);
        }
    }
}

void SimpleGossipProtocol::ProcessQueue() {
    while (!msgQueue.empty()) {
        P2PMessage msg = msgQueue.front();
        
        RunGossipProtocol(msg);

        msgQueue.pop();
    }
}
