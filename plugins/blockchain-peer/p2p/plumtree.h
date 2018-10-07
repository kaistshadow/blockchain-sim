#ifndef GOSSIP_PROTOCOL
#define GOSSIP_PROTOCOL

#include <string>
#include <vector>
#include "socketmessage.h"
#include "p2pmessage.h"

class GossipProtocol {
 private:
  GossipProtocol() {};                  // singleton pattern
  static GossipProtocol* instance;      // singleton pattern
    
 public:
  static GossipProtocol* GetInstance(); // sigleton pattern

  // variables and functions below are for mID and storage
  std::vector<int> receivedMsgs;
  size_t CreateMsgId(P2PMessage msg);
  int    CheckAlreadyReceived(P2PMessage msg);

  // Do eager/lazy setting to maintain gossip overlay
  // Also transfer tx/blk/csmsg to proper interface
  void RunGossipProtocol(SocketMessage msg);

  // process p2pmsgs from upper layer
  // use BROADCAST method to broadcast p2pmessages
  void ProcessQueue();
};

#endif // GOSSIP_PROTOCOL
