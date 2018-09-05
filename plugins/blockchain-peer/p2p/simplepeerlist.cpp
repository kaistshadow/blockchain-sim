#include <iostream>
#include <string>

#include "membershipmessage.h"
#include "p2pmessage.h"
#include "socketmessage.h"
#include "simplepeerlist.h"
#include "socket.h"

SimplePeerList* SimplePeerList::instance = 0;
SimplePeerList* SimplePeerList::GetInstance() {
  if (instance == 0) {
    instance = new SimplePeerList();
  }
  return instance;
}

void SimplePeerList::InitPeerList(int argc, char* argv[]) {
  for (int i=2; i<argc; i++) {    
    MembershipMessage msg = MembershipMessage(P_JOIN, 1, 1, std::string(argv[1]));
    P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
    
    SocketMessage smsg = SocketMessage();
    smsg.SetDstPeer(std::string(argv[i]));
    smsg.SetMethod(M_CONNECT, -1);
    smsg.SetP2PMessage(pmsg);
    SocketInterface::GetInstance()->PushToQueue(smsg);
  }
}

void SimplePeerList::AddToActive(Peer node) {
  active_view.push_back(node);
  PrintActive();
}

void SimplePeerList::PrintActive() {
  if (active_view.size()) {
    std::cout << "---Active view---\n";
    for (int i=0; i<active_view.size(); i++) {
      std::cout << active_view[i].peername << ", " << active_view[i].sfd << '\n';     
    }
    std::cout << "-----------------\n\n";
  }
} 
