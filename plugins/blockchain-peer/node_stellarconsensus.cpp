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
#include "util/types.h"

#include "crypto/sha256.h"

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

    // utility::globalclock_start = time(0);
    utility::globalclock_start = std::chrono::high_resolution_clock::now();
}

void NodeLoop() {
    utility::UINT128_t a(0,0x10);
    utility::UINT128_t b(0,17);
    utility::UINT128_t c = a * b;
    utility::UINT256_t d = c * 1000;
    unsigned char input[2] = {0x12,0x34};
    utility::UINT256_t e(input, 2);
    cout << "UINT128_t a = " << a  << "\n";
    cout << "UINT128_t b = " << b  << "\n";
    cout << "UINT128_t c = " << c  << "\n";
    cout << "UINT256_t d = " << d  << "\n";
    cout << "UINT256_t e = " << e  << "\n";
    if (a > b)
        cout << "a is bigger" << "\n";
    else
        cout << "b is bigger than or equal to a" << "\n"; 

    unsigned char hash_in1[2] = {0x11, 0x22};
    unsigned char hash_in2[2] = {0x33, 0x44};
    unsigned char hash_in3[4] = {0x11, 0x22, 0x33, 0x44};
    unsigned char hash_out1[32];
    unsigned char hash_out2[32];

    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, hash_in1, 2);
    sha256_update(&ctx, hash_in2, 2);
    sha256_final(&ctx, hash_out1);
    
    utility::UINT256_t hash_out1_int(hash_out1, 32);
    cout << "hash_out1_int:" << hash_out1_int << "\n";
    
    sha256_init(&ctx);
    sha256_update(&ctx, hash_in3, 4);
    sha256_final(&ctx, hash_out2);
    utility::UINT256_t hash_out2_int(hash_out2, 32);
    cout << "hash_out2_int:" << hash_out2_int << "\n";

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
