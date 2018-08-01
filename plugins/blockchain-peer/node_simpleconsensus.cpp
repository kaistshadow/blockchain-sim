#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "p2p/simplepeerlist.h"
#include "p2p/gossipprotocol.h"
#include "p2p/nodeinfos.h"

#include "consensus/simpleconsensus.h"

using namespace std;

void NodeInit(int nodeid);

void NodeLoop();

int main(int argc, char *argv[]) {
    int nodeid = atoi(argv[1]);
    cout << "Blockchain peer " << nodeid << " started!" << endl;

    NodeInit(nodeid);
    NodeLoop();
}

void NodeInit(int nodeid) {
    // initialize node id
    SetLocalNodeId(nodeid);

    // initialize the list of neighbor nodes
    InitializePeerList();
}

void NodeLoop() {
    while (true) {
        usleep(100000);

        // ideal gossip protocol
        RunGossipProtocol();
            
        // ideal consensus protocol
        RunConsensusProtocol(GetLocalNodeId());
    }
    return;
}
