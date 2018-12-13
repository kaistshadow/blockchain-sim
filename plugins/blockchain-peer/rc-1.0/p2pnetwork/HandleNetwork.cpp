#include "HandleNetwork.h"
#include "HandleNetworkTest.h"

std::shared_ptr<HandleNetwork>  HandleNetwork::create(const int type) {
    if (type == HANDLE_NETWORK_TEST) {
        return std::shared_ptr<HandleNetwork>(new HandleNetworkTest());
    }
    else {
        exit(-1);
    }
}
