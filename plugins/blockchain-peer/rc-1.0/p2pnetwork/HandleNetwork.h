#ifndef HANDLE_NETWORK_H
#define HANDLE_NETWORK_H

#include <memory>
#include "Message.h"
#include "../event/Subscriber.h"
#include "../event/Publisher.h"

#define HANDLE_NETWORK_PROXYMODEL 0
#define HANDLE_NETWORK_GOSSIPMODEL 1

class HandleNetwork {
 public:
    HandleNetwork();

    //Event Publisher
    SocketEventPublisher socketEventPublisher;
    //Event Subscribers
    Subscriber recvSocketConnectionEventSubscriber;
    Subscriber recvSocketDataEventSubscriber;
    Subscriber sendSocketReadyEventSubscriber;

    virtual ~HandleNetwork();
    static std::shared_ptr<HandleNetwork> create(const int type);

    virtual int JoinNetwork() = 0;

    virtual void UnicastMsg(std::string destip, Message* msg) = 0;
    virtual void BroadcastMsg(Message* msg) = 0;

    // Event subscriber callback 1. for recvSocketConnectionEvent
    virtual void onRecvSocketConnectionEvent(std::shared_ptr<EventInfo>) = 0;
    // Event subscriber callback 2. for recvSocketDataEvent
    virtual void onRecvSocketDataEvent(std::shared_ptr<EventInfo>) = 0;
    // Event subscriber callback 3. for sendSocketReadyEvent
    virtual void onSendSocketReadyEvent(std::shared_ptr<EventInfo>) = 0;
};


#endif // HANDLE_NETWORK_H
