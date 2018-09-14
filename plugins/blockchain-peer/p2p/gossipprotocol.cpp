#include <boost/variant.hpp>
#include <iostream>
#include <vector>
#include <string>

#include "gossipprotocol.h"
#include "simplepeerlist.h"
#include "socket.h"
#include "membershipmessage.h"
#include "p2pmessage.h"
#include "socketmessage.h"

#include "../blockchain/txpool.h"
#include "../consensus/simpleconsensus.h"

int CNT = 0;

SimpleGossipProtocol* SimpleGossipProtocol::instance = 0;
SimpleGossipProtocol* SimpleGossipProtocol::GetInstance() {
  if (instance == 0) {
    instance = new SimpleGossipProtocol();
  }
  return instance;
}

std::vector<std::string> SimpleGossipProtocol::SetShuffleList(int na, int np) {
  SimplePeerList* instance = SimplePeerList::GetInstance();
  PeerList active_view  = instance->active_view;
  PeerList passive_view = instance->passive_view;
  std::vector<std::string> list;

  int num_a = (na >= active_view.size()) ? active_view.size() :na;
  int num_p = (np >= passive_view.size())? passive_view.size():np;

  list.push_back(HostId);

  //Have to choose randomly
  for (int i=0; i<num_a; i++)
    list.push_back(active_view[i].peername);

  for (int i=0; i<num_p; i++)
    list.push_back(passive_view[i].peername);

  return list;
}

void SimpleGossipProtocol::SendShuffleMessage() {
  if (!SHUFFLE_OP || TEST) return;
  
  SimplePeerList* instance = SimplePeerList::GetInstance();
  PeerList     active_view = instance->active_view;
  if (active_view.size() == 0) return;

  MembershipMessage msg = MembershipMessage(P_SHUFFLE, 1, 1, GetHostId());
  msg.shuffle_list = SetShuffleList(Ka, Kp);

  P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
  SocketMessage    smsg = SocketMessage();
  smsg.SetP2PMessage(pmsg);

  srand(time(0));
  int idx = rand() % active_view.size();
  int  fd = active_view[idx].sfd;
  smsg.SetDstPeer(active_view[idx].peername);
  smsg.SetMethod(M_UNICAST, fd); 
  SocketInterface::GetInstance()->PushToQueue(smsg);
}

int SimpleGossipProtocol::ProcessShuffleList(std::vector<std::string> list) {
  SimplePeerList* instance = SimplePeerList::GetInstance();
  int         cnt; 
  std::string newid;

  for (cnt = 0; cnt < list.size(); cnt++) {
    newid = list[cnt];

    if (HostId == newid) continue;
    if (instance->ExistInActiveById(newid)  != -1) continue;    
    if (instance->ExistInPassiveById(newid) != -1) continue;
    Peer newpassive = Peer(newid, -1);
    instance->AddToPassive(newpassive);
  }
  return cnt;
}

void SimpleGossipProtocol::RunMembershipProtocol(SocketMessage msg) {
  // We only need 'soket_fd', and 'p2pmessage' to process HyParView
  int sfd   = msg.GetSocketfd();
  int stype = msg.GetMethod();
  MembershipMessage hmsg = boost::get<MembershipMessage>(msg.GetP2PMessage().data);
  
  int type = hmsg.type;
  int  ttl = hmsg.ttl;
  int  opt = hmsg.opt;
  std::string src = hmsg.src_peer;
 
  
  if (stype == M_UPDATE) {
    SocketMessage smsg = SocketMessage();
    smsg.SetDstPeer(src);
    smsg.SetMethod(M_UPDATE, sfd);
    SocketInterface::GetInstance()->PushToQueue(smsg);
  }

  switch (type) {
    case P_JOIN:
      // 1. Add src peer to active_view
      // 2. Reply by sending P_JOINREPLY
      // 3. Broadcast FORWARDJOIN p2p msg 
      {
	Peer newactive = Peer(src, sfd);
	SimplePeerList::GetInstance()->AddToActive(newactive);
	
	MembershipMessage msg = MembershipMessage(P_JOINREPLY, 1, 1, GetHostId());
	P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
	SocketMessage    smsg = SocketMessage();
	smsg.SetDstPeer(src);
	smsg.SetMethod(M_UNICAST, sfd);
	smsg.SetP2PMessage(pmsg);
	SocketInterface::GetInstance()->PushToQueue(smsg);
     
	if (TEST) break;
	if (SimplePeerList::GetInstance()->active_view.size() >= 2) {
	  MembershipMessage msg2 = MembershipMessage(P_FORWARDJOIN, 1, 1, src);
	  P2PMessage       pmsg2 = P2PMessage(P2PMessage_MEMBERSHIP, msg2);
	  SocketMessage    smsg2 = SocketMessage();
	  smsg2.SetMethod(M_BROADCAST, sfd);
	  smsg2.SetP2PMessage(pmsg2);
	  SocketInterface::GetInstance()->PushToQueue(smsg2);
	}
      }
      break;
      
    case P_JOINREPLY:
      // 1. this msg means src peer add this host into active view
      // 2. so insert src peer into host's active view too
      {
	Peer newactive = Peer(src, sfd);
	SimplePeerList::GetInstance()->AddToActive(newactive);
      }
      break;

    case P_DISCONNECT:
      // remove src peer from active and insert drop node into passive
      // if passive is not empty choose random from passvie and send neighbor
      {
	int idx = SimplePeerList::GetInstance()->ExistInActiveById(src);
	if (idx == -1)
	  return;

	SimplePeerList* instance = SimplePeerList::GetInstance();
	Peer dropnode = instance->active_view[idx];
	
	instance->DropFromActiveById(dropnode.peername);
	instance->AddToPassive(dropnode);
	
	srand(time(0));
	idx = rand() % (int)instance->passive_view.size();
	int pri = instance->active_view.size()==0 ? 1:0;
	int fd  = instance->passive_view[idx].sfd;
      
	MembershipMessage msg = MembershipMessage(P_NEIGHBOR, 1, pri, GetHostId());
	P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
	SocketMessage    smsg = SocketMessage();
	if (fd == -1) {
	  smsg.SetMethod(M_CONNECT, -1);
	}
	else {
	  smsg.SetMethod(M_UNICAST, fd);
	}
	smsg.SetDstPeer(instance->passive_view[idx].peername);	
	smsg.SetP2PMessage(pmsg);
	SocketInterface::GetInstance()->PushToQueue(smsg);
      }
      break;
   
    case P_FORWARDJOIN:
      // 1. check AddActive  condition
      // 2. check AddPassvie condition
      // 3. increase ttl and transfer to random peer inside active view
      {
	PeerList active_view = SimplePeerList::GetInstance()->active_view;
	if (ttl == ARWL || active_view.size() == 1) {
	  if (GetHostId() == src) 
	    return;
	  if (SimplePeerList::GetInstance()->ExistInActiveById(src) == -1) {
	    MembershipMessage msg = MembershipMessage(P_FORWARDJOINREPLY, 1, 1, GetHostId());
	    P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
	    SocketMessage    smsg = SocketMessage();
	    smsg.SetDstPeer(src);
	    smsg.SetP2PMessage(pmsg);
	     
	    int idx = SimplePeerList::GetInstance()->ExistInPassiveById(src);
	    if (idx == -1) {
	      smsg.SetMethod(M_CONNECT, -1);
	    }
	    else {
	      int fd = SimplePeerList::GetInstance()->passive_view[idx].sfd;
	      if (fd == -1) 
		smsg.SetMethod(M_CONNECT, -1);
	      else 
		smsg.SetMethod(M_UNICAST, fd);
	    }
	    SocketInterface::GetInstance()->PushToQueue(smsg);
	  }
	  return;
	}

	if (ttl == PRWL) {
	  if (GetHostId() == src) 
	    return;
	  Peer newpassive = Peer(src, -1);
	  SimplePeerList::GetInstance()->AddToPassive(newpassive);
	}
	
	srand(time(0));
	int idx, fd; 
	while (1) {
	  idx = rand() % active_view.size();
       	  if ((fd = active_view[idx].sfd) != sfd) break;
	}
	MembershipMessage msg = MembershipMessage(P_FORWARDJOIN, ttl+1, 1, src);
	P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
	SocketMessage    smsg = SocketMessage();
	smsg.SetMethod(M_UNICAST, fd);
	smsg.SetP2PMessage(pmsg);
	SocketInterface::GetInstance()->PushToQueue(smsg);
      }
      break;

    case P_FORWARDJOINREPLY:
      // If opt == 1,
      // 1. if ttl == 1, add to active_view and reply with ttl = 2
      // 2. if ttl == 2, just add to active view
      // If opt == 0, reply with ++ttl, opt=0
      {
	if (ttl == 1) {
	  int idx = SimplePeerList::GetInstance()->ExistInActiveById(src);
	  if (idx == -1) {
	    Peer newactive = Peer(src, sfd);
	    SimplePeerList::GetInstance()->AddToActive(newactive);
	  
	    MembershipMessage msg = MembershipMessage(P_FORWARDJOINREPLY, 2, 1, GetHostId());
	    P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
	    SocketMessage    smsg = SocketMessage();
	    smsg.SetMethod(M_UNICAST, sfd);
	    smsg.SetP2PMessage(pmsg);
	    SocketInterface::GetInstance()->PushToQueue(smsg);
	  }
	}
	if (ttl == 2) {
	  Peer newactive = Peer(src, sfd);
	  SimplePeerList::GetInstance()->AddToActive(newactive);
	}
      }
      break;

    case P_NEIGHBOR:
      // 1. if host's active view is full, then only high pri will be accepted
      // 2, else accept all
      {
	SimplePeerList* instance = SimplePeerList::GetInstance();
	int accept = 1;
	if (instance->active_view.size() == ActiveSize && opt == 0) {
	  accept = 0;
	}
	
     	if (accept) {
	  Peer newactive = Peer(src, sfd);
	  instance->AddToActive(newactive);
	}

	MembershipMessage msg = MembershipMessage(P_NEIGHBORREPLY, 1, accept, GetHostId());
	P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
	SocketMessage    smsg = SocketMessage();
	smsg.SetMethod(M_UNICAST, sfd);
	smsg.SetP2PMessage(pmsg);
	SocketInterface::GetInstance()->PushToQueue(smsg);	
      }
      break;

    case P_NEIGHBORREPLY:
      // 1. if opt == 1, then add src peer into active
      // 2. else, send NEIGHBOR to another candidate from passive
      {
	SimplePeerList* instance = SimplePeerList::GetInstance();
	if (opt) {
	  Peer newactive = Peer(src, sfd);
	  instance->AddToActive(newactive);
	  return;
	}
	
	int disconnect = 1;
	int idx = instance->ExistInPassiveById(src);
       	if (idx != -1) {
	  if (instance->passive_view[idx].sfd != -1) {
	    disconnect = 0;
	  }
	}
	if (disconnect) {
	  SocketMessage smsg = SocketMessage();
	  smsg.SetDstPeer(src);
	  smsg.SetMethod(M_DISCONNECT, sfd);
	  SocketInterface::GetInstance()->PushToQueue(smsg); 
	}
	
	int size = (int)instance->passive_view.size();
	if (size == 0) return;
	if (size == 1) idx = 0;
	else {
	  srand(time(0));
	  while (1) {
	    idx = rand() % size;
	    if (instance->passive_view[idx].peername != src) break;
	  }
	}

	int pri = instance->active_view.size()==0 ? 1:0;
	int fd  = instance->passive_view[idx].sfd; 
	MembershipMessage msg = MembershipMessage(P_NEIGHBOR, 1, pri, GetHostId());
	P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
	SocketMessage    smsg = SocketMessage();
	if (fd == -1) {
	  smsg.SetDstPeer(instance->passive_view[idx].peername);
	  smsg.SetMethod(M_CONNECT, fd);
	}
	else {
	  smsg.SetMethod(M_UNICAST, fd);
	}	   
	smsg.SetP2PMessage(pmsg);
	SocketInterface::GetInstance()->PushToQueue(smsg);
      }
      break;

    case P_SHUFFLE:
      // if ttl == 5, check the shuffle list from src and fill in host's passive view
      // And then, send shuffle reply to src
      // else, randomly propagate the msg to neighbor inside active view
      {
      	SimplePeerList* instance = SimplePeerList::GetInstance();
	if (ttl == 5 || instance->active_view.size() == 1) {	   
	  if (GetHostId() != src) {
	    MembershipMessage msg = MembershipMessage(P_SHUFFLEREPLY, 1, 1, GetHostId());
	    int cnt = ProcessShuffleList(hmsg.shuffle_list);
	    msg.shuffle_list = SetShuffleList(0, cnt-1);
	    
	    P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
	    SocketMessage    smsg = SocketMessage();
	    smsg.SetDstPeer(src);
	    smsg.SetP2PMessage(pmsg);
	    
	    int idx;
	    if ((idx = instance->ExistInActiveById(src)) != -1) {
	      smsg.SetMethod(M_UNICAST, instance->active_view[idx].sfd);
	    }
	    else if ((idx = instance->ExistInPassiveById(src)) != -1){
	      int fd = instance->passive_view[idx].sfd;
	      if (fd == -1) {
		smsg.SetMethod(M_CONNECT|M_DISCONNECT, -1);
	      }
	      else {
		smsg.SetMethod(M_UNICAST, fd);
	      }
	    }
	    else {
	      smsg.SetMethod(M_CONNECT|M_DISCONNECT, -1);
	    }
	    SocketInterface::GetInstance()->PushToQueue(smsg);
	  }
	  return;
	}

	PeerList active_view = instance->active_view;
	int idx, fd; 
	srand(time(0)); 
	while (1) {
	  idx = rand() % active_view.size();
       	  if ((fd = active_view[idx].sfd) != sfd) break;
	}
       	
	hmsg.ttl++;
	P2PMessage    pmsg = P2PMessage(P2PMessage_MEMBERSHIP, hmsg);
	SocketMessage smsg = SocketMessage();
	smsg.SetMethod(M_UNICAST, fd);
	smsg.SetP2PMessage(pmsg);
	SocketInterface::GetInstance()->PushToQueue(smsg);
      }
      break;

    case P_SHUFFLEREPLY:
      // if receive this msg, then insert all node inside msg into passive view
      // if there are duplicated node, then skip that one ONLY insert new one
      // if passive view is full, then drop random node (or front = oldest)
      {
	ProcessShuffleList(hmsg.shuffle_list);
      }
      break;
      
    default:
      break;
  }
}

void SimpleGossipProtocol::RunGossipProtocol(SocketMessage msg) {
  int stype = msg.GetMethod();

  if (stype == M_NETWORKFAIL) {
    int sfd = msg.GetSocketfd();
    if (sfd <= 0) {
      std::cerr << "strange network fail socekt\n"; 
      return;
    }
      
    SimplePeerList* instance = SimplePeerList::GetInstance();    
    int idx = instance->ExistInActive(sfd);
    if (idx != -1) {
      Peer dropnode = instance->active_view[idx];
      instance->DropFromActiveById(dropnode.peername);
     
      dropnode.sfd = -1;
      instance->AddToPassive(dropnode);
    
      srand(time(0));
      idx = rand() % (int)instance->passive_view.size();
      int pri = instance->active_view.size()==0 ? 1:0;
      int fd  = instance->passive_view[idx].sfd;
      MembershipMessage msg = MembershipMessage(P_NEIGHBOR, 1, pri, GetHostId());
      P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
      SocketMessage    smsg = SocketMessage();
      if (fd == -1) {
	smsg.SetDstPeer(instance->passive_view[idx].peername);
	smsg.SetMethod(M_CONNECT, -1);
      }
      else {
	smsg.SetMethod(M_UNICAST, fd);
      }	
      smsg.SetP2PMessage(pmsg);
      SocketInterface::GetInstance()->PushToQueue(smsg);
      return;
    }
    
    idx = instance->ExistInPassive(sfd);
    if (idx =! -1) {
      instance->passive_view[idx].sfd = -1;    
    }
    return;
  }

  P2PMessage pmsg = msg.GetP2PMessage();
  switch(pmsg.type) {
    case P2PMessage_MEMBERSHIP:
      {
	RunMembershipProtocol(msg);
      }
      break;
   
    case P2PMessage_TRANSACTION:
      {	
	if (!CNT) {
	  msg.SetMethod(M_BROADCAST, msg.GetSocketfd());
	  SocketInterface::GetInstance()->PushToQueue(msg);
	  CNT = 1;
	  std::cout << "tx recv\n";
	}
      }
      break;
      
    case P2PMessage_BLOCK:
      {;}
      break;

    case P2PMessage_SIMPLECONSENSUSMESSAGE:
      {;}
      break;      
  }
}

void SimpleGossipProtocol::ProcessQueue() {
  while (!msgQueue.empty()) {
    SocketMessage msg = msgQueue.front();     
    RunGossipProtocol(msg);
    msgQueue.pop();
  }
}
