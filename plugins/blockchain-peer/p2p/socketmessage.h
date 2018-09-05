#ifndef NETWORK_MESSAGE_H
#define NETWORK_MESSAGE_H

#include <string>
#include <vector>
#include "p2pmessage.h"

typedef enum method {
  M_NETWORKFAIL = 0, // NOT use protocol->socket direction
  M_BROADCAST   = 1,
  M_UNICAST     = 2,
  M_CONNECT     = 4,
  M_DISCONNECT  = 8
} Method;

class SocketMessage{
 public:
  SocketMessage() {};
    
  std::string GetDstPeer() {return dst_peer;}
  int         GetSocketfd(){return socket_fd;}
  int         GetMethod()  {return method;}
  P2PMessage  GetP2PMessage() {return p2pmessage;}

  void SetDstPeer(std::string pn) {dst_peer  = pn;}
  void SetSocketfd(int sfd)       {socket_fd = sfd;}  
  void SetP2PMessage(P2PMessage msg) {p2pmessage = msg;}
  
  // ** HAVE TO USE THIS FUNCTION **
  void SetMethod(int m, int fd); 

  // list of sfd used for broad/uni cast.
  // automatically be filled in by using "SetMethod" funtion
  std::vector<int> sockets; 

 private:
  int         socket_fd;  // socket fd for dst peer (for specific target) 
  std::string dst_peer;   // name of destination peer (Use for M_CONNECT)
  int         method;     // work that socket interface have to do
  P2PMessage  p2pmessage; 
}; 

#endif // NETWORK_MESSAGE_H
