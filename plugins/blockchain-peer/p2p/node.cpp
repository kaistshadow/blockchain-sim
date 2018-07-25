#include <unistd.h>

#include "node.h"
#include "simplepeerlist.h"
#include "gossipprotocol.h"
#include "../consensus/simpleconsensus.h"

int NodeId = -1;

void NodeInit(int nodeid) {
    // initialize node id
    NodeId = nodeid;

    // initialize the list of neighbor nodes
    InitializePeerList();
}

void NodeLoop() {
    while (true) {
        usleep(100000);

        // 
        RunGossipProtocol();
            
        //
        RunConsensusProtocol(NodeId);
    }
    return;
}
