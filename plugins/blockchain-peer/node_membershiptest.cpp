#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "p2p/simplepeerlist.h"
#include "p2p/gossipprotocol.h"
#include "p2p/socket.h"

using namespace std;

void NodeInit(int argc, char *argv[]);
void NodeLoop();

int main(int argc, char *argv[]) {
  //cout << "Blockchain Peer start!\n";
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
  //cout << "initialization Done!\n";
}

void NodeLoop() {
  time_t start = time(0);
  cout << "Node Loop start!\n\n";

  int cnt = 0;
  while (true) {
    usleep(100000);
    
    if (++cnt%5 == 0) SimpleGossipProtocol::GetInstance()->SendShuffleMessage();
    SocketInterface::GetInstance()->ProcessQueue();          
    SocketInterface::GetInstance()->ProcessNetworkEvent();
    SimpleGossipProtocol::GetInstance()->ProcessQueue();
  }
}
