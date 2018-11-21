#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "p2p/centralized/peerlistmanager_combined.h"
#include "p2p/centralized/asyncsocketinterface.h"
#include "p2p/centralized/centralizedmsgproxy_async.h"

#include "blockchain/txpool.h"
#include "blockchain/ledgermanager.h"
#include "blockchain/powledgermanager.h"

#include "consensus/powconsensus_centralized_async.h"

#include "util/eventqueue.h"
#include "util/globalclock.h"
#include "util/types.h"
#include "util/logmanager.h"

#include "crypto/sha256.h"

using namespace std;

void NodeInit(int argc, char *argv[]);

void NodeLoop(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    // int nodeid = atoi(argv[1]);
    cout << "Blockchain peer for PoW consensus" << " started!" << "\n";

    NodeInit(argc, argv);
    NodeLoop(argc, argv);
}

// assume that command arguments are given as follows
// ./command <my_node_id> <node_id_of_neighboring_p2p_node> ...
void NodeInit(int argc, char *argv[]) {
    // 1. Initialize the list of out peers 
    // PeerListManagerCombined::GetInstance()->InitializeOutPeerList(argc-2, &argv[2]);
    PeerListManagerCombined::GetInstance()->InitializeMyPeer(argv[1]);
 
    // 2. Initialize network socket
    AsyncSocketInterface::GetInstance()->InitializeListenSocket();


    // initialize blockchain (currently from file.) 
    // TODO: initialize blockchain by retrieving live blockchain from network
    POWLedgerManager::SetInstance("blk.dat");
    POWLedgerManager::GetInstance()->LoadLedgerFromFile();
    

    utility::globalclock_start = std::chrono::high_resolution_clock::now();
    utility::logger_opt = LOGGER_OFF;

    // Set Node host id
    NodeInfo::GetInstance()->SetHostId(std::string(argv[1]));

}

void NodeLoop(int argc, char *argv[]) {
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

    utility::UINT256_t temp = utility::UINT256_t(hash_out2, 32);
    cout << "temp:" << hash_out2_int << "\n";

    AsyncSocketInterface::GetInstance()->SetProcessingQueueType(PROCESSINGQUEUETYPE_NODE);

    struct ev_loop *loop = ev_default_loop(0);
    AsyncSocketInterface::GetInstance()->SetEvLoop(loop);    
    POWConsensus::GetInstance()->SetEvLoop(loop);

    AsyncSocketInterface::GetInstance()->RegisterServerWatcher();
    AsyncSocketInterface::GetInstance()->RegisterPeriodicConnectWatcher(argc-2, &argv[2]);

    ev_loop(EV_A_ 0); // start event loop


    // while (true) {
    //     usleep(100000);
        // cout << "globalclock:" << utility::GetGlobalClock() << "\n";

        // process synchronous event queue
        // EventQueueManager::GetInstance()->ProcessQueue();


        // process non-blocking network socket events
        // process non-blocking accept and non-blocking recv
        // PeerList& inPeerList = SimplePeerListManager::GetInstance()->GetInPeerList();
        // PeerList& outPeerList = SimplePeerListManager::GetInstance()->GetOutPeerList();
        // SimpleSocketInterface::GetInstance()->ProcessNonblockSocket(inPeerList, outPeerList);


        // consensus protocol
        // POWConsensus::GetInstance()->Run();
        // RunConsensusProtocol(GetLocalNodeId());  # TODO 2
        // SimpleConsensus::GetInstance()->RunConsensusProtocol();


        // Process pending messages in messageQueues for each module
        // TxPool::GetInstance()->ProcessQueue();
        // CentralizedMessageProxy::GetInstance()->ProcessQueue();
        // POWConsensus::GetInstance()->ProcessQueue();

    // }
    return;
}
