#include "HandleNetwork.h"
#include "HandleNetwork_ProxyModel.h"

std::shared_ptr<HandleNetwork>  HandleNetwork::create(const int type) {
    if (type == HANDLE_NETWORK_PROXYMODEL) {
        return std::shared_ptr<HandleNetwork>(new HandleNetwork_ProxyModel());
    }
    else {
        exit(-1);
    }
}
