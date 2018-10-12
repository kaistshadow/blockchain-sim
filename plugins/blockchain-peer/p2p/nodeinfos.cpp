#include "nodeinfos.h"

static int NodeId = -1;

void SetLocalNodeId(int nodeid) {
    // set node id
    NodeId = nodeid;

}

int GetLocalNodeId() {
    return NodeId;
}

