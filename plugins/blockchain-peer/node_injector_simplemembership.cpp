#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/wait.h> 
#include <arpa/inet.h>
#include <netdb.h>
#include <map>
#include <vector>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>

#include "p2p/socket.h"
#include "p2p/p2pmessage.h"
#include "p2p/socketmessage.h"
#include "p2p/plumtree.h"

#include "blockchain/transaction.h"
#include "consensus/simpleconsensusmessage.h"
using namespace std;

#define MYPORT 3456    /* the port users will be connecting to */
#define BACKLOG 10     /* how many pending connections queue will hold */

map <std::string, int> socket_m;
void NodeInit(int argc, char *argv[]);
void NodeLoop(char *argv[]);

int main(int argc, char *argv[]) {
  cout << "simpleconsensus injector started!\n";
  NodeInit(argc, argv);
  NodeLoop(argv);
}

int connect_to_node(std::string pn) {
  struct addrinfo* peerinfo;
  int res = getaddrinfo((const char*)pn.c_str(), NULL, NULL, &peerinfo);
  if (res == -1) return -1;

  in_addr_t ip = ((struct sockaddr_in*)(peerinfo->ai_addr))->sin_addr.s_addr;
  freeaddrinfo(peerinfo);

  int sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) return -1;

  struct sockaddr_in address;
  memset(&address, 0, sizeof(address));
  address.sin_family      = AF_INET;
  address.sin_port        = htons(MYPORT);
  address.sin_addr.s_addr = ip;

  res = connect(sfd, (struct sockaddr*)&address, sizeof(address));
  if (res == -1) {
    close(sfd); return -1;
  }

  std::cout << "connection established\n";
  socket_m.insert(make_pair(pn, sfd));
  return sfd;
}

void send_SocketMessage(int sfd, SocketMessage msg) {
  std::string payload = GetSerializedString(msg.GetP2PMessage());
  int     payload_len = payload.size();
  if (payload_len <= 0) {
    std::cerr << "send event: Serialization fault\n";
    return;
  }

  int numbytes = send(sfd, (char*)&payload_len, sizeof(int), 0);
  if (numbytes < sizeof(int)) {
    std::cerr << "send event: network fail\n";
    return;
  }
  numbytes = send(sfd, payload.c_str(), payload_len, 0);
  if (numbytes < payload_len) {
    std::cerr << "send event: network fail\n";
    return;
  }
  std::cout << "send done\n";
}

void InjectTransaction(int sfd, int from, int to, double value) {
  Transaction tx(from, to, value);
  P2PMessage  p2pmessage(P2PMessage_TRANSACTION, tx);
  p2pmessage.g_mid = GossipProtocol::GetInstance()->CreateMsgId(p2pmessage);

  SocketMessage msg;
  msg.SetSocketfd(sfd);
  msg.SetP2PMessage(p2pmessage);

  send_SocketMessage(sfd, msg);
}

void NodeInit(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    connect_to_node(std::string(argv[i]));
  }
}

void NodeLoop(char *argv[]) {
  {
    // 1. create Transaction and inject to first node
    int sfd = socket_m.find(std::string(argv[1]))->second;
    InjectTransaction(sfd, 0, 1, 12.34);
    InjectTransaction(sfd, 0,2,10);
    InjectTransaction(sfd, 1,3,10);
    InjectTransaction(sfd, 2,0,11);
    InjectTransaction(sfd, 1,0,12.34);
  }

  {
    // 2. Create Block and inject to first node
    int sfd = socket_m.find(argv[1])->second;
    std::list<Transaction> tx_list;

    Transaction tx(0,1,123.45); // send 123.45 to node 1 from node 0
    tx_list.push_back(tx);
    Transaction tx2(3,2,43.21); // send 43.21 to node 2 from node 3
    tx_list.push_back(tx2);
    Block block("injected block 0", tx_list);
        
    P2PMessage p2pmessage(P2PMessage_BLOCK, block);
    p2pmessage.g_mid = GossipProtocol::GetInstance()->CreateMsgId(p2pmessage);

    SocketMessage msg;
    msg.SetSocketfd(sfd);
    msg.SetP2PMessage(p2pmessage);
    send_SocketMessage(sfd, msg);

    cout << "Following block will be injected" << "\n";
  }

  {
    // 3. Quorum initialization and Leader Election.
    for (auto it = socket_m.begin(); it != socket_m.end(); it++) {
      std::string node_id = it->first;
      int sfd = it->second;
    
      SimpleConsensusMessage consensusMsg(SimpleConsensusMessage_INIT_QUORUM, node_id); 
      P2PMessage p2pmessage(P2PMessage_SIMPLECONSENSUSMESSAGE, consensusMsg);
      p2pmessage.g_mid = GossipProtocol::GetInstance()->CreateMsgId(p2pmessage);
      
      SocketMessage msg;
      msg.SetSocketfd(sfd);
      msg.SetP2PMessage(p2pmessage);
      send_SocketMessage(sfd, msg);
    }

    std::string node_id(argv[1]);
    int sfd = socket_m.find(argv[1])->second;
    
    SimpleConsensusMessage consensusMsg(SimpleConsensusMessage_LEADER_ELECTION, node_id); 
    P2PMessage p2pmessage(P2PMessage_SIMPLECONSENSUSMESSAGE, consensusMsg);
    p2pmessage.g_mid = GossipProtocol::GetInstance()->CreateMsgId(p2pmessage);
   
    SocketMessage msg;
    msg.SetSocketfd(sfd);
    msg.SetP2PMessage(p2pmessage);
    send_SocketMessage(sfd, msg);
  }
  return;
}
