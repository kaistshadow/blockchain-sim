#include <iostream>
#include <string>
#include <queue>
#include <tr1/functional>

#include "plumtree.h"
#include "p2pmessage.h"
#include "socketmessage.h"
#include "simplepeerlist.h"
#include "gossipprotocol.h"
#include "socket.h"

#include "../blockchain/txpool.h"
#include "../consensus/simpleconsensus.h"
#include "../consensus/stellarconsensus.h"
#include "../consensus/powconsensus.h"
#include "../blockchain/powledgermanager.h"

#include <boost/variant.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/device/array.hpp>
using namespace boost::archive;

GossipProtocol* GossipProtocol::instance = 0;
GossipProtocol* GossipProtocol::GetInstance() {
  if (instance == 0) {
    instance = new GossipProtocol();
  }
  return instance;
}

std::string ConvertToString(P2PMessage msg) {
  std::string serial_str;
  boost::iostreams::back_insert_device<std::string> inserter(serial_str);
  boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
  boost::archive::binary_oarchive oa(s);
  oa << msg;
  s.flush();
  return serial_str;
}

size_t GossipProtocol::CreateMsgId(P2PMessage msg) {
  std::string sstr = ConvertToString(msg);
  std::tr1::hash<std::string> str_hash;
  std::string ssfd = SimpleGossipProtocol::GetInstance()->GetHostId();
  return str_hash(sstr + ssfd);
}

int GossipProtocol::CheckAlreadyReceived(P2PMessage msg) {
  size_t mid = msg.g_mid;
  for (int i=0; i<receivedMsgs.size(); i++) {
    if (receivedMsgs[i] == mid) return 1;
  }
  return 0;
}

void GossipProtocol::RunGossipProtocol(SocketMessage msg) {
  P2PMessage pmsg = msg.GetP2PMessage();
  int    sfd = msg.GetSocketfd();
  int g_type = pmsg.g_type;
  PeerList* active_view = &SimplePeerList::GetInstance()->active_view;

  // if receiving GOSSIP type msgs, then need to check host already received it or not
  // if yes, the link that gave the msg to host is slow path so set sender as Lazy
  // if no, means first time, then since it is fast link so set sender as Eager
  // And push mid of new msg into "receivedMsgs"
  // g_types like IHAVE, GRAFT are used to optimize/recover gossip overlay
  // so we do not need to implement right now
  if (g_type == GOSSIP) {
    int   res = CheckAlreadyReceived(pmsg);
    int exist = 0;
    
    for (int i=0; i<active_view->size(); i++) {
      if (sfd == (*active_view)[i].sfd) {
	if ((*active_view)[i].state == Eager) {
	  (*active_view)[i].state = (res == 0)? Eager:Lazy;
	  /*
	  if (res) {
	    if (pmsg.type == P2PMessage_POWCONSENSUSMESSAGE)
	      std::cout << "@@Prune(CS) to "<< (*active_view)[i].peername <<" due to "<<pmsg.g_mid<<"\n";
	    if (pmsg.type == P2PMessage_TRANSACTION)
	      std::cout << "@@Prune(TX) to "<< (*active_view)[i].peername <<" due to "<<pmsg.g_mid<<"\n";
	  }
	  */
	}
	else {
	  // receive new msg through Lazy edge
	  if (!res) {
	    std::cerr << "RunGossip: Recv New msg through Lazy edge\n";
	    return;
   	  } 
	}
	SimplePeerList::GetInstance()->PrintActive();
	exist = 1;
	break;
      }
    }
    if (!exist) {
      std::cerr << "RunGossip: No such socket in active\n";   
    }

    if (!res) { 
      receivedMsgs.push_back(pmsg.g_mid);
    }
    else { 
      //send PRUNE to change opposite's edge state to Lazy
      P2PMessage pmsg;
      pmsg.g_type  = PRUNE;
      pmsg.g_mid   = 0;
      pmsg.g_round = 0;
      
      SocketMessage smsg;
      smsg.SetP2PMessage(pmsg);
      smsg.SetMethod(M_UNICAST, sfd);
      SocketInterface::GetInstance()->PushToQueue(smsg);
      // Do we need to broadcast already recieved msg? check the paper 
      return;
    }
  }
  else if (g_type == PRUNE) {    
    int exist = 0;
    for (int i=0; i<active_view->size(); i++) {
      if (sfd == (*active_view)[i].sfd) {
	(*active_view)[i].state = Lazy;
	SimplePeerList::GetInstance()->PrintActive();
	exist = 1;
	break;
      }
    }
    if (!exist) 
      std::cerr << "RunGossip: No such socket in active(prune)\n";
    return;
  }
  else { 
    std::cerr << "RunGossip: Not use IHAVE,GRAFT yet\n";
    return;
  }

  // For msgs which have g_type as GOSSIP, then transfer them to proper interface
  // others, IHAVA/GRAFT aren't transfered to upper interface
  // because they are only used to set gossip overlay 
  switch(pmsg.type) {
    case P2PMessage_TRANSACTION: 
      {
	Transaction tx = boost::get<Transaction>(pmsg.data);
	TxPool::GetInstance()->PushTxToQueue(tx);
	//std::cerr << "recv new tx\n";
      }
      break;

    case P2PMessage_BLOCK:
      {
	//std::cerr << "recv new blk\n";
        // Must be propagated to proper ledgermanager.
        // Currently, do thing.

        Block *blk = boost::get<Block>(&pmsg.data);
        if (blk) {
            std::cout << "Following block is received" << "\n";
            std::cout << *blk << "\n";
        }
        else {
            std::cout << "Wrong data in P2PMessage" << "\n";
            exit(1);
        }
      }
      break;

    case P2PMessage_POWCONSENSUSMESSAGE:
      {
        POWConsensusMessage powmsg = boost::get<POWConsensusMessage>(pmsg.data);
        POWConsensus::GetInstance()->PushToQueue(powmsg);
      }
      break;

    case P2PMessage_STELLARCONSENSUSMESSAGE:
      {
        // Not implemented yet. Even Not updated for plumtree
        bool broadcast = false;

        StellarConsensusMessage *consensusMsg = boost::get<StellarConsensusMessage>(&pmsg.data);
        if (consensusMsg) {

          if ( !pmsg.IsProcessedByNode(StellarConsensus::GetInstance()->GetNodeId()) ) {
            StellarConsensus::GetInstance()->PushToQueue(*consensusMsg);          
            pmsg.SetProcessedByNode(StellarConsensus::GetInstance()->GetNodeId());
          }  
            
          if (consensusMsg->type != StellarConsensusMessage_INIT_QUORUM && pmsg.GetHopCount() < 4 )
            broadcast = true;
        } else {
          std::cout << "Wrong data in P2PMessage STELLARCONSENSUSMESSAGE" << "\n";
          exit(1);
        }
        // propagate a message to p2p network.
        // if (broadcast) {
        //   msg.IncreaseHopCount();
        //   PeerList outPeerList = SimplePeerList::GetInstance()->GetOutPeerList();
        //   for (PeerList::iterator it = outPeerList.begin(); it != outPeerList.end(); it++) {    
        //     Peer* p = *it;
        //     if (p->conn_status == CONNECTED) {
        //       // create SocketMessage and insert into the queue
        //       SocketMessage socketMsg;
        //       socketMsg.SetSocketfd(p->sfd);
        //       socketMsg.SetP2PMessage(msg);
        //       std::string payload = GetSerializedString(socketMsg);
        //       socketMsg.SetPayload(payload);

        //       SocketInterface::GetInstance()->PushToQueue(socketMsg);
        //     }
        //   }
        // }
      }

    case P2PMessage_SIMPLECONSENSUSMESSAGE:
      {
	SimpleConsensusMessage cmsg = boost::get<SimpleConsensusMessage>(pmsg.data);
	SimpleConsensus::GetInstance()->PushToQueue(cmsg);
	//std::cerr << "recv new css msg\n";
      }
      break;
      
    default:
      break;
  }

  // Broadcast to whole peer which have Eager state
  // In this case, the "g_mid" does not change and also "g_type"
  // Yet, we do not use "g_round" so using BROADCAST is fine
  msg.SetMethod(BROADCAST, sfd);
  SocketInterface::GetInstance()->PushToQueue(msg);
}

void GossipProtocol::ProcessQueue() {
  // Need to process msgQueue2 which stores msgs from above interface
  // just make new SocketMessage for each P2PMessages
  // Set method as BROADCAST and exception as 0 then it will broadcast to whole eager
  // without any exception peer.
  std::queue<P2PMessage> *queue = SimpleGossipProtocol::GetInstance()->GetUpperQueue();

  while(!queue->empty()){
    P2PMessage pmsg = queue->front();
    pmsg.g_mid = CreateMsgId(pmsg);

    // check created mid alreay exists in storage
    // if not, inset it into the storage
    // else, print error log since its not allowed
    int res = CheckAlreadyReceived(pmsg);
    if (!res) { 
      receivedMsgs.push_back(pmsg.g_mid);
      std::cout << "@@mid: "<<pmsg.g_mid<<"\n";
    }
    else {
      std::cerr << "@@PlumTree: broadcast already known mid\n";
    }
    
    SocketMessage msg = SocketMessage();
    msg.SetP2PMessage(pmsg);
    msg.SetMethod(BROADCAST, 0);

    SocketInterface::GetInstance()->PushToQueue(msg);
    queue->pop();
  }
}
