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

#include "plumtree.h"
#include "../blockchain/txpool.h"
#include "../consensus/simpleconsensus.h"
#include "../consensus/stellarconsensus.h"

#include "../blockchain/powledgermanager.h"

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
  
  list.clear();
  list.push_back(HostId);
  //Have to choose randomly
  for (int i=0; i<num_a; i++)
    list.push_back(active_view[i].peername);

  for (int i=0; i<num_p; i++)
    list.push_back(passive_view[i].peername);

  /*
  srand(time(0));
  while (num_a) {
    int idx = rand() % active_view.size();
    int res = 1;
    for (int i = 0; i<list.size(); i++) {
      if (list[i] == active_view[idx].peername) {
	res = 0;
	break;
      }
    }
    if (res) {
      list.push_back(active_view[idx].peername);
      num_a -= 1;
    }
  }

  srand(time(0));
  while (num_p) {
    int idx = rand() % passive_view.size();
    int res = 1;
    for (int i = 0; i<list.size(); i++) {
      if (list[i] == passive_view[idx].peername) {
	res = 0;
	break;
      }
    }
    if (res) {
      list.push_back(passive_view[idx].peername);
      num_p -= 1;
    }
  }
  */
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
  std::string newid;
  int cnt = 0; 
  
  for (int i = 0; i < list.size(); i++) {
    newid = list[i];
    if (HostId == newid) continue;
    if (instance->ExistInActiveById(newid)  != -1) continue;    
    if (instance->ExistInPassiveById(newid) != -1) continue;
    Peer newpassive = Peer(newid, -1);
    instance->AddToPassive(newpassive);
    cnt++;
  }
  return cnt;
}

void SimpleGossipProtocol::RunMembershipProtocol(SocketMessage msg) {
  int sfd   = msg.GetSocketfd();
  int stype = msg.GetMethod();
  MembershipMessage hmsg = boost::get<MembershipMessage>(msg.GetP2PMessage().data);
  
  int type = hmsg.type;
  int  ttl = hmsg.ttl;
  int  opt = hmsg.opt;
  std::string src = hmsg.src_peer;
   
  if (stype == M_UPDATE) 
    // Transfer Update msg with peer id to socket interface
    {
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
      // Choose random from passvie and send NEIGHBOR
      // size of passive view can not be 0 in this case.
      {
	int idx = SimplePeerList::GetInstance()->ExistInActiveById(src);
	if (idx == -1) {
	  std::cerr<< "recv P_DISCONNECT from non-active member("<<src<<")\n"; 
	  return;
	}
	
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
      // 3. increase ttl and transfer to single random peer inside active view
      {
	PeerList active_view = SimplePeerList::GetInstance()->active_view;
	if (ttl == ARWL || active_view.size() == 1) {
	  if (GetHostId() == src) return;
	  if (SimplePeerList::GetInstance()->ExistInActiveById(src) == -1) {
	    MembershipMessage msg = MembershipMessage(P_FORWARDJOINREPLY, 1, 1, GetHostId());
	    P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
	    SocketMessage    smsg = SocketMessage();
	    smsg.SetDstPeer(src);
	    smsg.SetP2PMessage(pmsg);
	     
	    int idx = SimplePeerList::GetInstance()->ExistInPassiveById(src);
	    if (idx == -1) { // most case, idx == -1
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
	else if (ttl == PRWL) {
	  if (GetHostId() != src) { 
	    Peer newpassive = Peer(src, -1);
	    SimplePeerList::GetInstance()->AddToPassive(newpassive);
	  }
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
      // Have to reduce 3 steps to 2 steps
      // 1. if ttl == 1, add to active_view and reply with ttl = 2
      // 2. if ttl == 2, just add to active view
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
	if (instance->ExistInActiveById(src) != -1) {
	  std::cerr << "Neighbor : "<<src<<" already exists inside active\n";
	}
	else {
	  if (instance->active_view.size() == ActiveSize && opt == 0) {
	    accept = 0;
	  }
	  if (accept) {
	    Peer newactive = Peer(src, sfd);
	    instance->AddToActive(newactive);
	  }
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
	if (instance->ExistInActiveById(src) != -1) {
	  std::cerr << "Neighbor reply : "<<src<<" already exists inside active\n";
	  return;
	}
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
	    //if (instance->passive_view[idx].sfd != sfd)
	    //  std::cout << "Neighbor reply : diff socket#\n";
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
	    // first, check existence of connection between host and src
	    int idx;
	    int fd = -1;
	    int HaveConnection = 0;
	    if ((idx = instance->ExistInActiveById(src)) != -1) {
	      fd = instance->active_view[idx].sfd;
	      HaveConnection = 1;
	    }
	    else if ((idx = instance->ExistInPassiveById(src)) != -1){
	      fd = instance->passive_view[idx].sfd;
	      HaveConnection = (fd == -1)? 0:1;
	    }

	    // then, use HaveConnection as opt and process shuffle list from src
	    MembershipMessage msg = MembershipMessage(P_SHUFFLEREPLY, 1, HaveConnection, GetHostId());
	    int cnt = ProcessShuffleList(hmsg.shuffle_list);
	    msg.shuffle_list = SetShuffleList(0, cnt-1);
	    
	    P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
	    SocketMessage    smsg = SocketMessage();
	    smsg.SetDstPeer(src);
	    smsg.SetP2PMessage(pmsg);
	    if (HaveConnection) {
	      smsg.SetMethod(M_UNICAST, fd);
	    }
	    else {
	      smsg.SetMethod(M_CONNECT, -1);
	    }
	    SocketInterface::GetInstance()->PushToQueue(smsg);
	  }
	  return;
	}

	// forward the msg to other active node
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
      	SimplePeerList* instance = SimplePeerList::GetInstance();
	// first, check existence of connection between host and src
	int idx;
	int HaveConnection = 0;
	if ((idx = instance->ExistInActiveById(src)) != -1) {
	  HaveConnection = 1;
	}
	else if ((idx = instance->ExistInPassiveById(src)) != -1){
	  HaveConnection = (instance->passive_view[idx].sfd == sfd)? 1:0;
	}

	// then, process shuffle list from src, sender
	ProcessShuffleList(hmsg.shuffle_list);
	 
	// last, determine whether close the connection or not
	if (!(opt || HaveConnection)) {
	  SocketMessage smsg = SocketMessage();
	  smsg.SetDstPeer(src);
	  smsg.SetMethod(M_DISCONNECT, sfd);
	  SocketInterface::GetInstance()->PushToQueue(smsg);
	}
      }
      break;
      
    default:
      break;
  }
}

void SimpleGossipProtocol::RunGossipInterface(SocketMessage msg) {
  int stype = msg.GetMethod();

  // Network faiurel is dealt with Membership protocol
  // Tier down when node inside partial view becomes fail
  if (stype == M_NETWORKFAIL) {
    int sfd = msg.GetSocketfd();
    if (sfd <= 0) {
      std::cerr << "networkfail: invalid socekt number\n"; 
      return;
    }
    
    SimplePeerList* instance = SimplePeerList::GetInstance();    
    int idx = instance->ExistInActive(sfd);
    if (idx != -1) {
      Peer dropnode = instance->active_view[idx];
      dropnode.sfd = -1;
      instance->DropFromActiveById(dropnode.peername);//ensure same entry?
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
      std::cerr << "networkfail: tier down from active to passive\n"; 
      return;
    }
    
    idx = instance->ExistInPassive(sfd);
    if (idx =! -1) {
      instance->passive_view[idx].sfd = -1;    
      std::cerr << "networkfail: change sfd of problematic passvie node\n"; 
    }
    return;
  }

  // if type == MEMBERSHIP, then run membership protocol HyParView
  // else, then run gossip protocol PlumTree
  int p2pmessage_type = msg.GetP2PMessage().type;
  if (p2pmessage_type == P2PMessage_MEMBERSHIP) {
    RunMembershipProtocol(msg);
  }
  else {
    GossipProtocol::GetInstance()->RunGossipProtocol(msg);
  }
}

void SimpleGossipProtocol::ProcessQueue() {
  if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().\
							    time_since_epoch()).count() % 50== 0) 
    SendShuffleMessage();
 
  while (!msgQueue.empty()) {
    SocketMessage msg = msgQueue.front();     
    RunGossipInterface(msg);
    msgQueue.pop();
  }
}
