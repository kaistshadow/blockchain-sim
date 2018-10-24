#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "p2p/simplepeerlist.h"
#include "p2p/gossipprotocol.h"
// #include "p2p/nodeinfos.h"
#include "p2p/socket.h"
#include "p2p/plumtree.h"
#include "blockchain/txpool.h"
#include "util/nodeinfo.h"

#include "consensus/simpleconsensus.h"

using namespace std;

void NodeInit(int argc, char *argv[]);

void NodeLoop();

int main(int argc, char *argv[]) {
    // int nodeid = atoi(argv[1]);
    cout << "Blockchain peer " << " started!" << "\n";

    NodeInit(argc, argv);
    NodeLoop();
}

void NodeInit(int argc, char *argv[]) {
    // 1. Initialize Socket Interface
    SocketInterface::GetInstance()->InitSocketInterface();
 
    // 2. Initialize Protocol Interface
    SimpleGossipProtocol::GetInstance()->InitProtocolInterface(argv[1]);
 
    // 3. Initailisze Peerlist
    SimplePeerList::GetInstance()->InitPeerList(argc, argv); 

    // initialize the list of neighbor nodes
    // SimplePeerList::GetInstance()->AddPeerList(servhostname);  

    // PeerList outPeerList = SimplePeerList::GetInstance()->GetOutPeerList();

    // // initialize network socket
    // SocketInterface::GetInstance()->InitializeSocket(outPeerList);

    // Set Node host id
    NodeInfo::GetInstance()->SetHostId(std::string(argv[1]));
}

void NodeLoop() {
    time_t start = time(0);
    while (true) {
        usleep(100000);
        // cout << "time:" << difftime(time(0), start) << "\n";

        // process non-blocking network socket events
        // process non-blocking accept and non-blocking recv
        // PeerList inPeerList = SimplePeerList::GetInstance()->GetInPeerList();
        // PeerList outPeerList = SimplePeerList::GetInstance()->GetOutPeerList();
        // SocketInterface::GetInstance()->ProcessNonblockSocket(inPeerList, outPeerList);

        // Socket Layer
        SocketInterface::GetInstance()->ProcessQueue();          
        SocketInterface::GetInstance()->ProcessNetworkEvent();

        
            
        // ideal consensus protocol
        // RunConsensusProtocol(GetLocalNodeId());  # TODO 2
        SimpleConsensus::GetInstance()->RunConsensusProtocol();


        // Process pending messages in messageQueues for each module
        TxPool::GetInstance()->ProcessQueue();
        SimpleGossipProtocol::GetInstance()->ProcessQueue();
        SimpleConsensus::GetInstance()->ProcessQueue();

        // Gossip Layer:
        // process Upper queue of Gossip Layer which store p2pmsgs from Consensus Layer
        GossipProtocol::GetInstance()->ProcessQueue();
    }
    return;
}
