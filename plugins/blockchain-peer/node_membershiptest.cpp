#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "p2p/simplepeerlist.h"
#include "p2p/plumtree.h"
#include "p2p/gossipprotocol.h"
#include "p2p/socket.h"

#include "consensus/simpleconsensus.h"
using namespace std;

void NodeInit(int argc, char *argv[]);
void NodeLoop();

int main(int argc, char *argv[]) {
  NodeInit(argc,argv);
  NodeLoop();
}

void NodeInit(int argc, char *argv[]) {
  // 1. Initialize Socket Interface
  SocketInterface::GetInstance()->InitSocketInterface();
 
  // 2. Initialize Protocol Interface
  SimpleGossipProtocol::GetInstance()->InitProtocolInterface(argv[1]);
 
  // 3. Initailisze Peerlist
  SimplePeerList::GetInstance()->InitPeerList(argc, argv); 
}

void NodeLoop() {
  time_t start = time(0);
  cout << "Node Loop start!\n\n";

  int cnt = 0;
  while (true) {
    usleep(100000);
    
    // Cyclon protocol which belongs to HyParView 
    //if (++cnt%5 == 0) SimpleGossipProtocol::GetInstance()->SendShuffleMessage();

    // Socket Layer
    SocketInterface::GetInstance()->ProcessQueue();          
    SocketInterface::GetInstance()->ProcessNetworkEvent();

    // Gossip Layer
    // process Bottom queue of Gossip Layer which store p2pmsgs from Consensus Layer
    SimpleGossipProtocol::GetInstance()->ProcessQueue();
   
    // Consensus Layer
    SimpleConsensus::GetInstance()->ProcessQueue();
    SimpleConsensus::GetInstance()->RunConsensusProtocol();
    
    // Gossip Layer:
    // process Upper queue of Gossip Layer which store p2pmsgs from Consensus Layer
    GossipProtocol::GetInstance()->ProcessQueue();
  }
}
