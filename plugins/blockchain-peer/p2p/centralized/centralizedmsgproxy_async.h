#ifndef CENTRALIZED_MSG_PROXY_ASYNC_H
#define CENTRALIZED_MSG_PROXY_ASYNC_H

#include <queue>

#include "peerlistmanager_combined.h"
#include "broadcastrequestmessage.h"
#include "unicastrequestmessage.h"
#include "proxygeneratedmessage.h"


class CentralizedMessageProxyAsync {
 private:
    CentralizedMessageProxyAsync() {}; //singleton pattern
    static CentralizedMessageProxyAsync* instance; // singleton pattern

    std::queue<BroadcastRequestMessage> broadcastMsgQueue;
    std::queue<UnicastRequestMessage> unicastMsgQueue;

    void SendOverP2PNetwork(ProxyGeneratedMessage& msg);

 public:
    static CentralizedMessageProxyAsync* GetInstance(); // singleton pattern

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
    bool ProcessQueue();
};






#endif // CENTRALIZED_MSG_PROXY_ASYNC_H
