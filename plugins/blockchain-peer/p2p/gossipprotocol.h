#ifndef P2P_GOSSIPPROTOCOL_H
#define P2P_GOSSIPPROTOCOL_H

#include <queue>
#include <string>
#include "socketmessage.h"

#define ARWL 6
#define PRWL 3

#define Ka 3
#define Kp 4

#define TEST 0

class SimpleGossipProtocol {
 private:
  SimpleGossipProtocol() {};                  // singleton pattern
  static SimpleGossipProtocol* instance;      // singleton pattern

  std::queue<SocketMessage> msgQueue;
  std::string HostId;

 public:
  static SimpleGossipProtocol* GetInstance(); // singleton pattern
  
  void InitProtocolInterface (char* id){
    HostId = std::string(id);
  }
  std::string GetHostId() {return HostId;}

  void RunMembershipProtocol(SocketMessage msg);
  void RunGossipProtocol(SocketMessage msg);

  void PushToQueue(SocketMessage msg) {msgQueue.push(msg);}
  void ProcessQueue();
};

#endif // P2P_GOSSIPPROTOCOL_H
