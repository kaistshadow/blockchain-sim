#include <vector>
#include "simplepeerlist.h"
#include "socketmessage.h"

void SocketMessage::SetMethod(int m, int fd) {
  method = m;
  if (fd == -1 || method == M_CONNECT) 
    return;
  if (method == M_NETWORKFAIL || method == M_UPDATE || method == M_NORMAL) {
    socket_fd = fd;
    return;
  }

  sockets.clear();
  // M_BROADCAST : for membership, use whole active_view
  // BROADCAST   : for gossip, use only eager state subset of active_view
  // For this method, input fd is "exception" which means broadcast without this fd
  if (method == M_BROADCAST || method == BROADCAST) {
    PeerList active_view = SimplePeerList::GetInstance()->active_view;

    for (int i=0; i<active_view.size(); i++) {
      int  temp = active_view[i].sfd;
      int state = active_view[i].state;
     
      if (fd == temp) continue;
      if (method == BROADCAST && state == Lazy) continue; 
      sockets.push_back(temp);
    }
    socket_fd = -1;
    return;
  }

  // Set sockets with only one sfd
  if (method == M_UNICAST || method & M_DISCONNECT) {
    sockets.push_back(fd);
    return;
  }
}
