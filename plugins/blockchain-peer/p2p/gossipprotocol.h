#ifndef P2P_GOSSIPPROTOCOL_H
#define P2P_GOSSIPPROTOCOL_H

#include <queue>
#include <vector>
#include <string>
#include "socketmessage.h"
#include "p2pmessage.h"

#define ARWL 6//3//6
#define PRWL 3//1//3
#define Ka 3
#define Kp 4

#define TEST 0
#define SHUFFLE_OP 0

class SimpleGossipProtocol {
 private:
  SimpleGossipProtocol() {};                  // singleton pattern
  static SimpleGossipProtocol* instance;      // singleton pattern

  std::queue<SocketMessage> msgQueue;
  std::queue<P2PMessage>    msgUpperQueue;
  std::string HostId;

  std::vector<std::string> SetShuffleList(int na, int np);
  int  ProcessShuffleList(std::vector<std::string> list);

 public:
  static SimpleGossipProtocol* GetInstance(); // singleton pattern
  
  void InitProtocolInterface (char* id) {HostId = std::string(id);}
  std::string GetHostId() {return HostId;}

  void SendShuffleMessage();
  void RunMembershipProtocol(SocketMessage msg);
  void RunGossipInterface(SocketMessage msg);

  // Queue for msgs from below, Socket Interface
  void PushToQueue(SocketMessage msg) {msgQueue.push(msg);}
  void ProcessQueue();

  // Queue for msgs from above, Consensus Layer
  // Processing is perfomed using GossipProtocol class function
  void PushToUpperQueue(P2PMessage msg) {msgUpperQueue.push(msg);}
  std::queue<P2PMessage>* GetUpperQueue() {return &msgUpperQueue;}
};

#endif // P2P_GOSSIPPROTOCOL_H
