#ifndef P2P_GOSSIPPROTOCOL_H
#define P2P_GOSSIPPROTOCOL_H

#include <queue>

#include "simplepeerlist.h"
#include "p2pmessage.h"

class SimpleGossipProtocol {
 private:
    SimpleGossipProtocol() {}; //singleton pattern
    static SimpleGossipProtocol* instance; // singleton pattern

    std::queue<P2PMessage> msgQueue;
 public:
    static SimpleGossipProtocol* GetInstance(); // singleton pattern

    /**
     * Run gossip protocol.
     * Receive any pending requests from inPeerList, and broadcast them to outPeerList.
     */
    void RunGossipProtocol(P2PMessage msg);

    /** 
     * Push message into message queue.
     */
    void PushToQueue(P2PMessage msg) { msgQueue.push(msg); }
    void ProcessQueue();
};





















#endif // P2P_GOSSIPPROTOCOL_H
