#include "nodeinfo.h"

NodeInfo* NodeInfo::instance = 0;

NodeInfo* NodeInfo::GetInstance() {
    if (instance == 0) {
        instance = new NodeInfo();
    }
    return instance;
}
