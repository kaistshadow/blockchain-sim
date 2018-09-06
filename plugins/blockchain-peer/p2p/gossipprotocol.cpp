#include <boost/variant.hpp>
#include <iostream>
#include <string>

#include "gossipprotocol.h"
#include "simplepeerlist.h"
#include "socket.h"
#include "membershipmessage.h"
#include "p2pmessage.h"
#include "socketmessage.h"

#include "../blockchain/txpool.h"
#include "../consensus/simpleconsensus.h"

SimpleGossipProtocol* SimpleGossipProtocol::instance = 0;
SimpleGossipProtocol* SimpleGossipProtocol::GetInstance() {
  if (instance == 0) {
    instance = new SimpleGossipProtocol();
  }
  return instance;
}

void SimpleGossipProtocol::RunMembershipProtocol(SocketMessage msg) {
  // We only need 'soket_fd', and 'p2pmessage' to process HyParView
  int sfd = msg.GetSocketfd();
  MembershipMessage hmsg = boost::get<MembershipMessage>(msg.GetP2PMessage().data);
  
  int type = hmsg.type;
  int  ttl = hmsg.ttl;
  int  opt = hmsg.opt;
  std::string src = hmsg.src_peer;
 
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
      {
	int idx;
	if ((idx = SimplePeerList::GetInstance()->ExistInActive(sfd)) == -1)
	  return;

	SimplePeerList* instance = SimplePeerList::GetInstance();
	Peer dropnode = instance->active_view[idx];
	instance->DropFromActive(dropnode.sfd);
	instance->AddToPassive(dropnode);
      }
      break;
   
    case P_FORWARDJOIN:
      // 1. check AddActive  condition
      // 2. check AddPassvie condition
      // 3. increase ttl and transfer to random peer inside active view
      {
	PeerList active_view = SimplePeerList::GetInstance()->active_view;
	if (GetHostId() != src) {
	  if (ttl == ARWL || active_view.size() == 1) {
	    if (SimplePeerList::GetInstance()->ExistInActiveById(src) == -1) {
	      MembershipMessage msg = MembershipMessage(P_FORWARDJOINREPLY, 1, 1, GetHostId());
	      P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
	      SocketMessage    smsg = SocketMessage();
	      smsg.SetDstPeer(src);
	      smsg.SetMethod(M_CONNECT, sfd);
	      smsg.SetP2PMessage(pmsg);
	      SocketInterface::GetInstance()->PushToQueue(smsg);
	      return;
	    }
	  }
	  if (ttl == PRWL) {
	    Peer newpassive = Peer(src, -1);
	    SimplePeerList::GetInstance()->AddToPassive(newpassive);
	  }
	}
	if (ttl == ARWL || active_view.size() == 1) return;

	srand(time(0));
	int idx, fd; 
	while (1) {
	  idx = rand() % active_view.size();
       	  if (active_view[idx].sfd != sfd) {
	    fd = active_view[idx].sfd;
	    break;
	  }
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
	  
	    MembershipMessage msg = MembershipMessage(P_FORWARDJOINREPLY, ttl+1, 1, GetHostId());
	    P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
	    SocketMessage    smsg = SocketMessage();
	    smsg.SetMethod(M_UNICAST, sfd);
	    smsg.SetP2PMessage(pmsg);
	    SocketInterface::GetInstance()->PushToQueue(smsg);
	  }
	  else if (SimplePeerList::GetInstance()->active_view[idx].sfd != sfd) {
	    SocketMessage smsg = SocketMessage();
	    smsg.SetMethod(M_DISCONNECT, sfd);
	    SocketInterface::GetInstance()->PushToQueue(smsg);
	  }
	}
	else if (ttl == 2) {
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
	  accept == 0;
	}
	
	MembershipMessage msg = MembershipMessage(P_NEIGHBORREPLY, 1, accept, GetHostId());
	P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
	SocketMessage    smsg = SocketMessage();
	smsg.SetMethod(M_UNICAST, sfd);
	smsg.SetP2PMessage(pmsg);
	SocketInterface::GetInstance()->PushToQueue(smsg);
	
     	if (accept) {
	  Peer newactive = Peer(src, sfd);
	  instance->AddToActive(newactive);
	}
	else {
	  int idx = instance->ExistInPassiveById(src);
	  if (idx != -1) {
	    instance->passive_view[idx].sfd = sfd;
	  }
	}
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
	}
	else {
	  if (instance->passive_view.size() == 1)
	    return;

	  srand(time(0));
	  int idx = sfd;
	  while (idx == sfd) { 
	    idx = rand() % (int)instance->passive_view.size();
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
      }
      break;

    case P_SHUFFLE:{;}
      break;
    case P_SHUFFLEREPLY:{;}
      break;
      
    default:
      break;
  }
}

void SimpleGossipProtocol::RunGossipProtocol(SocketMessage msg) {
 
  if (msg.GetMethod() == M_NETWORKFAIL) {
    int sfd = msg.GetSocketfd();
    SimplePeerList* instance =SimplePeerList::GetInstance();
    
    // Drop fail peer from active view and choose a random peer from passive
    // To insert that peer into active, send P_NEIGHBOR msg to the candidate
    int idx = instance->ExistInActive(sfd);
    if (idx != -1) {
      if (instance->active_view[idx].sfd != sfd)
	return;

      instance->active_view.erase(instance->active_view.begin() + idx);
      if (instance->passive_view.size() == 0) 
	return;
     
      srand(time(0));
      idx = rand() % (int)instance->passive_view.size();
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
      return;
    }
    // Drop fail peer from passive. if it is not in passive then nothing happen
    instance->DropFromPassive(sfd);
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
      {;}
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
