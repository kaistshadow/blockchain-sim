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
	
	MembershipMessage msg = MembershipMessage(P_JOINREPLY, 0, 0, GetHostId());
	P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
	SocketMessage    smsg = SocketMessage();
	smsg.SetDstPeer(src);
	smsg.SetMethod(M_UNICAST, sfd);
	smsg.SetP2PMessage(pmsg);
	SocketInterface::GetInstance()->PushToQueue(smsg);
     
	if (TEST) break;
	if (SimplePeerList::GetInstance()->active_view.size()>=2) {
	  MembershipMessage msg2 = MembershipMessage(P_FORWARDJOIN, 1, 0, src);
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
   
    case P_FORWARDJOIN:
      // 1. check AddActive  condition
      // 2. check AddPassvie condition
      // 3. increase ttl and transfer to random peer inside active view
      {
	PeerList active_view = SimplePeerList::GetInstance()->active_view;
	if (ttl == ARWL || active_view.size() == 1) {
	  MembershipMessage msg = MembershipMessage(P_FORWARDJOINREPLY, 1, 1, GetHostId());
	  P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
	  SocketMessage    smsg = SocketMessage();
	  smsg.SetDstPeer(src);
	  smsg.SetMethod(M_CONNECT, sfd);
	  smsg.SetP2PMessage(pmsg);
	  SocketInterface::GetInstance()->PushToQueue(smsg);
	  break;
	}
	if (ttl == PRWL) {
	  Peer newpassive = Peer(src, -1);
	  SimplePeerList::GetInstance()->AddToPassive(newpassive);
	}
	
	int idx, fd; 
	srand(time(0));
	while (1) {
	  idx = rand() % active_view.size();
       	  if (active_view[idx].sfd != sfd) {
	    fd = active_view[idx].sfd;
	    break;
	  }
	}
	MembershipMessage msg = MembershipMessage(P_FORWARDJOIN, ttl+1, 0, src);
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
	  Peer newactive = Peer(src, sfd);
	  SimplePeerList::GetInstance()->AddToActive(newactive);

	  MembershipMessage msg = MembershipMessage(P_FORWARDJOINREPLY, ttl+1, 1, GetHostId());
	  P2PMessage       pmsg = P2PMessage(P2PMessage_MEMBERSHIP, msg);
	  SocketMessage    smsg = SocketMessage();
	  smsg.SetMethod(M_UNICAST, sfd);
	  smsg.SetP2PMessage(pmsg);
	  SocketInterface::GetInstance()->PushToQueue(smsg);
	}
	else if (ttl == 2) {
	  Peer newactive = Peer(src, sfd);
	  SimplePeerList::GetInstance()->AddToActive(newactive);
	}
      }
      break;

    default:
      break;
  }
}

void SimpleGossipProtocol::RunGossipProtocol(SocketMessage msg) {
  P2PMessage pmsg = msg.GetP2PMessage();
 
  if (pmsg.type == P2PMessage_MEMBERSHIP) {
    RunMembershipProtocol(msg);
    return;
  }
  // Add other type message processing
}

void SimpleGossipProtocol::ProcessQueue() {
  while (!msgQueue.empty()) {
    SocketMessage msg = msgQueue.front();     
    RunGossipProtocol(msg);
    msgQueue.pop();
  }
}
