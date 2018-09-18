#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "p2p/simplepeerlist.h"
#include "p2p/gossipprotocol.h"
#include "p2p/socket.h"
#include "blockchain/txpool.h"
#include "blockchain/ledgermanager.h"

#include "consensus/stellarconsensus2.h"
#include "consensus/stellarconsensusdriver.h"

#include "util/eventqueue.h"
#include "util/globalclock.h"
using namespace std;

StellarConsensusDriver stellarConsensusDriver;

void NodeInit(int argc, char *argv[]);

void NodeLoop();

int main(int argc, char *argv[]) {
    // int nodeid = atoi(argv[1]);
    cout << "Blockchain peer " << " started!" << "\n";

    NodeInit(argc, argv);
    NodeLoop();
}

// assume that command arguments are given as follows
// ./command <node_id_of_neighbor> <node_id_of_neighbor> ...
void NodeInit(int argc, char *argv[]) {

    for (int i = 1; i < argc; i++) {
        // add neighbor node 
        SimplePeerList::GetInstance()->AddPeerList(argv[i]);
    }
    
    // initialize network socket
    PeerList outPeerList = SimplePeerList::GetInstance()->GetOutPeerList();    
    SocketInterface::GetInstance()->InitializeSocket(outPeerList);

    // initialize blockchain (currently from file.) 
    // TODO: initialize blockchain by retrieving live blockchain from network

    LedgerManager::SetInstance(stellarConsensusDriver, "blk.dat");
    LedgerManager::GetInstance()->InitLedger();

    utility::globalclock_start = time(0);
}

void NodeLoop() {
    while (true) {
        usleep(100000);
        // cout << "globalclock:" << utility::GetGlobalClock() << "\n";

        // process synchronous event queue
        EventQueueManager::GetInstance()->ProcessQueue();


        // process non-blocking network socket events
        // process non-blocking accept and non-blocking recv
        PeerList inPeerList = SimplePeerList::GetInstance()->GetInPeerList();
        PeerList outPeerList = SimplePeerList::GetInstance()->GetOutPeerList();
        SocketInterface::GetInstance()->ProcessNonblockSocket(inPeerList, outPeerList);
        
            
        // LedgerManager has main logic to proceed consensus and append a new block
        LedgerManager::GetInstance()->Loop();

        // ideal consensus protocol
        // RunConsensusProtocol(GetLocalNodeId());  # TODO 2
        // SimpleConsensus::GetInstance()->RunConsensusProtocol();


        // Process pending messages in messageQueues for each module
        TxPool::GetInstance()->ProcessQueue();
        SimpleGossipProtocol::GetInstance()->ProcessQueue();
        SocketInterface::GetInstance()->ProcessQueue();
        StellarConsensus2::GetInstance()->ProcessQueue();
    }
    return;
}
