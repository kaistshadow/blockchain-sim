#include "HandleNetwork.h"
#include "HandleNetwork_ProxyModel.h"
#include "HandleNetwork_GossipModel.h"

HandleNetwork::HandleNetwork() :
    recvSocketConnectionEventSubscriber(EventCoordinator::GetInstance(),
                                        [this](std::shared_ptr<EventInfo> i) { onRecvSocketConnectionEvent(i); },
                                        EventType::recvSocketConnectionEvent),
    recvSocketDataEventSubscriber(EventCoordinator::GetInstance(),
                                        [this](std::shared_ptr<EventInfo> i) { onRecvSocketDataEvent(i); },
                                        EventType::recvSocketDataEvent),
    sendSocketReadyEventSubscriber(EventCoordinator::GetInstance(),
                                        [this](std::shared_ptr<EventInfo> i) { onSendSocketReadyEvent(i); },
                                        EventType::sendSocketReadyEvent)
    // refer to https://stackoverflow.com/a/402385 for understanding why i used lambda.
    // refer to https://stackoverflow.com/a/11284096 for understanding the way of using lambda in constructor.
{
}

HandleNetwork::~HandleNetwork() {
}

std::shared_ptr<HandleNetwork>  HandleNetwork::create(const int type) {
    if (type == HANDLE_NETWORK_PROXYMODEL) {
        return std::shared_ptr<HandleNetwork>(new HandleNetwork_ProxyModel());
    }
    else if (type == HANDLE_NETWORK_GOSSIPMODEL) {
        return std::shared_ptr<HandleNetwork>(HandleNetwork_GossipModel::GetInstance());
    }
    else {
        exit(-1);
    }
}
