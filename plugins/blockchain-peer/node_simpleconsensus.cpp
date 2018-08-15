#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "p2p/simplepeerlist.h"
#include "p2p/gossipprotocol.h"
// #include "p2p/nodeinfos.h"
#include "p2p/socket.h"

#include "consensus/simpleconsensus.h"

using namespace std;

void NodeInit(char *servhostname);

void NodeLoop();

int main(int argc, char *argv[]) {
    // int nodeid = atoi(argv[1]);
    cout << "Blockchain peer " << " started!" << "\n";

    NodeInit(argv[1]);
    NodeLoop();
}

void NodeInit(char *servhostname) {
    // initialize node id
    // SetLocalNodeId(nodeid);

    // initialize the list of neighbor nodes
    SimplePeerList::GetInstance()->InitializePeerList(servhostname);  

    PeerList outPeerList = SimplePeerList::GetInstance()->GetOutPeerList();

    // initialize network socket
    SocketInterface::GetInstance()->InitializeSocket(outPeerList);
}

void NodeLoop() {
    while (true) {
        usleep(100000);

        // process non-blocking network socket events
        // process non-blocking accept and non-blocking recv
        PeerList outPeerList = SimplePeerList::GetInstance()->GetOutPeerList();
        SocketInterface::GetInstance()->ProcessNonblockSocket(outPeerList);
        
        // ideal gossip protocol
        // RunGossipProtocol(); # TODO 1
            
        // ideal consensus protocol
        // RunConsensusProtocol(GetLocalNodeId());  # TODO 2
    }
    return;
}
