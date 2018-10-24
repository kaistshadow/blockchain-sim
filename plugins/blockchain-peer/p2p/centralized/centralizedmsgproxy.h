#ifndef CENTRALIZED_MSG_PROXY_H
#define CENTRALIZED_MSG_PROXY_H

#include <queue>

#include "peerlistmanager.h"
#include "broadcastrequestmessage.h"
#include "unicastrequestmessage.h"
#include "proxygeneratedmessage.h"


class CentralizedMessageProxy {
 private:
    CentralizedMessageProxy() {}; //singleton pattern
    static CentralizedMessageProxy* instance; // singleton pattern

    std::queue<BroadcastRequestMessage> broadcastMsgQueue;
    std::queue<UnicastRequestMessage> unicastMsgQueue;

    void SendOverP2PNetwork(ProxyGeneratedMessage& msg);

 public:
    static CentralizedMessageProxy* GetInstance(); // singleton pattern

    /* /\** */
    /*  * Initiate centralized broadcast. */
    /*  * Receive any p2p requests and broadcast them to outPeerList. */
    /*  *\/ */
    /* void Broadcast(CentralizedP2PMessage msg); */

    /** 
     * Push message into message queue.
     */
    void PushToQueue(BroadcastRequestMessage msg) { broadcastMsgQueue.push(msg); }
    void PushToQueue(UnicastRequestMessage msg) { unicastMsgQueue.push(msg); }
    void ProcessQueue();
};
















#endif // CENTRALIZED_GOSSIPPROTOCOL_H
