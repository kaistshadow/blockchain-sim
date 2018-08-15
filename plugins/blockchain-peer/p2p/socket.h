#ifndef P2P_SOCKET_H
#define P2P_SOCKET_H

#include <vector>

#include "simplepeerlist.h"

#define MYPORT 3456    /* the port users will be connecting to */
#define BACKLOG 10     /* how many pending connections queue will hold */

using namespace std;

class SocketInterface {
 private:
    SocketInterface(){};  // singleton pattern
    static SocketInterface* instance; // singleton pattern

    void InitServerSocket();
    void InitClientSocket(PeerList outPeerList);
    int server_sfd;
    
    vector<int> recv_sfd_list;  // socket ids for non-blocking receiving sockets
    vector<int> send_sfd_list;  // socket ids for connected sockets (sending)

 public:
    static SocketInterface* GetInstance(); // singleton pattern

    /**
     * Initialize the non-blocking socket for listening socket. 
     */
    void InitializeSocket(PeerList outPeerList);

    /**
     * Process socket events
     * 1. process non-blocking accept (and make recv_sockets)
     * 2. process non-blocking connect (and make send_sockets)
     * 3. process non-blocking recv
     */
    void ProcessNonblockSocket(PeerList outPeerList);

    /**
     * Set and Get a server socket
     */
    void SetListenSocket(int sfd);
    int GetListenSocket();
};















#endif // P2P_SOCKET_H
