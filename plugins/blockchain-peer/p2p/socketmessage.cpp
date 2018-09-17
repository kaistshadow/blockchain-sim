#include <vector>
#include "simplepeerlist.h"
#include "socketmessage.h"

void SocketMessage::SetMethod(int m, int fd) {
  method = m;
  if (fd == -1) return;

  if (method == M_NETWORKFAIL) {
    socket_fd = fd;
    return;
  }
  if (method == M_UPDATE) {
    socket_fd = fd;
    return;
  }
  if (method == M_CONNECT) {
    return;
  }

  if (method == M_BROADCAST) {
    PeerList active_view = SimplePeerList::GetInstance()->active_view;
    int temp;
    for (int i=0; i<active_view.size(); i++) {
      temp = active_view[i].sfd;
      if (fd != temp){
	sockets.push_back(temp);
      }
    }
    return;
  }

  if (method & M_UNICAST || method & M_DISCONNECT) {
    sockets.push_back(fd);
  }
}
