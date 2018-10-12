#include <iostream>
#include <string>

#include "membershipmessage.h"
#include "p2pmessage.h"
#include "socketmessage.h"
#include "simplepeerlist.h"
#include "gossipprotocol.h"
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
    SocketMessage    smsg = SocketMessage();
    smsg.SetDstPeer(std::string(argv[i]));
    smsg.SetMethod(M_CONNECT, -1);
    smsg.SetP2PMessage(pmsg);
    SocketInterface::GetInstance()->PushToQueue(smsg);
  }
}

// This function is used when active view is fulled and 
// try to add new node into it. (ex) JOIN, FORWARDJOIN
void SimplePeerList::DropRandomFromActive() {
  srand(time(0));
  int       idx = rand() % (int)active_view.size();
  Peer dropnode = active_view[idx];
  
  MembershipMessage msg = MembershipMessage(P_DISCONNECT, 1, 1, 
					    SimpleGossipProtocol::GetInstance()->GetHostId());
  P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
  SocketMessage    smsg = SocketMessage();
  smsg.SetDstPeer(dropnode.peername);
  smsg.SetMethod(M_UNICAST, dropnode.sfd);
  smsg.SetP2PMessage(pmsg);
  SocketInterface::GetInstance()->PushToQueue(smsg);
  
  active_view.erase(active_view.begin() + idx);

  PrintActive();  
  AddToPassive(dropnode);
}

void SimplePeerList::DropFromActiveById(std::string pn) {
  for (int i=0; i<active_view.size(); i++){
    if (active_view[i].peername == pn) {
      active_view.erase(active_view.begin() + i);
      break;
    }
  }
  PrintActive();
}

// this function is used when some active node is fail.
void SimplePeerList::DropFromActive(int fd) {
  for (int i=0; i<active_view.size(); i++){
    if (active_view[i].sfd == fd) {
      active_view.erase(active_view.begin() + i);
      break;
    }
  }
  PrintActive();
}

int SimplePeerList::ExistInActiveById(std::string pn) {
  for (int i=0; i<active_view.size(); i++){
    if (active_view[i].peername == pn) return i;
  }
  return -1;
}

int SimplePeerList::ExistInActive(int fd) {
  for (int i=0; i<active_view.size(); i++){
    if (active_view[i].sfd == fd) return i;
  }
  return -1;
}

// this function is used when host gets new node to be added.
void SimplePeerList::AddToActive(Peer node) {
  int idx = ExistInPassiveById(node.peername);
  if (idx != -1) {
    passive_view.erase(passive_view.begin() + idx);
    PrintPassive();
  }
  if (ExistInActiveById(node.peername) != -1) {
    return;
  }

  if (active_view.size() == ActiveSize) {
    DropRandomFromActive();
  }
  active_view.push_back(node);
  PrintActive();
}

void SimplePeerList::DropRandomFromPassive() {
  srand(time(0));
  int idx = rand() % (int)passive_view.size();
  passive_view.erase(passive_view.begin() + idx);
  PrintPassive();  
}

void SimplePeerList::DropFromPassive(int fd) {
  for (int i=0; i<passive_view.size(); i++){
    if (passive_view[i].sfd == fd) {
      passive_view.erase(passive_view.begin() + i);
      PrintPassive();
      return;
    }
  }
}

int SimplePeerList::ExistInPassiveById(std::string pn) {
  for (int i=0; i<passive_view.size(); i++){
    if (passive_view[i].peername == pn)
      return i;
  }
  return -1;
}

int SimplePeerList::ExistInPassive(int fd) {
  for (int i=0; i<passive_view.size(); i++){
    if (passive_view[i].sfd == fd)
      return i;
  }
  return -1;
}

void SimplePeerList::AddToPassive(Peer node) {
  if (ExistInActiveById(node.peername) != -1) {
    return;
  }
  if (ExistInPassiveById(node.peername) != -1)
    return;
  
  if (passive_view.size() == PassiveSize)
    passive_view.erase(passive_view.begin());

  passive_view.push_back(node);
  PrintPassive();
}

void SimplePeerList::PrintActive() {
  if (active_view.size() == 0) {
    std::cout << '\n';
    return;
  }

  for (int i=0; i<active_view.size(); i++) {
    if (i == active_view.size()-1) {      
      std::cout << active_view[i].peername <<"("<< active_view[i].state <<")"<< '\n';
    }
    else{
      std::cout << active_view[i].peername <<"("<< active_view[i].state <<")"<< ',';     
    }
  }
} 

void SimplePeerList::PrintPassive() {
  return;

  std::cerr << '\n';
  if (passive_view.size() == 0) {
    std::cerr << "None\n";
    return;
  }

  for (int i=0; i<passive_view.size(); i++) {
    if (i == passive_view.size()-1)
      std::cerr << passive_view[i].peername << '\n';
    else
      std::cerr << passive_view[i].peername << ',';
  }
} 
