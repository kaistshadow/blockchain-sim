#include "nodeinfo.h"

NodeInfo* NodeInfo::instance = 0;

NodeInfo* NodeInfo::GetInstance() {
    if (instance == 0) {
        instance = new NodeInfo();
    }
    return instance;
}

int NodeInfo::GetHostNumber() {
    if (myNodeId.find("bleep") == 0) {
        return std::stoi(myNodeId.substr(5, myNodeId.size()));
    }
    else
        return -1;
}
