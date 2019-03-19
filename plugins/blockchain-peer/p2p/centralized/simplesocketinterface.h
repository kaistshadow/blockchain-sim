#ifndef SIMPLE_SOCKET_INTERFACE_H
#define SIMPLE_SOCKET_INTERFACE_H

#include <vector>
#include <queue>

#include "peerlistmanager.h"
#include "centralizednetworkmessage.h"

#define MYPORT 3456    /* the port users will be connecting to */
#define BACKLOG 10     /* how many pending connections queue will hold */

using namespace std;

class SimpleSocketInterface {
 private:
    SimpleSocketInterface(){};  // singleton pattern
    static SimpleSocketInterface* instance; // singleton pattern

    void InitServerSocket();
    void InitClientSocket(PeerList& outPeerList);
    int server_sfd;
    
    vector<int> send_sfd_list;  // socket ids for connected sockets (sending)

    /**
     * Set and Get a server socket
     */
    void SetListenSocket(int sfd) { server_sfd = sfd; }
    int GetListenSocket() { return server_sfd; }

 public:
    static SimpleSocketInterface* GetInstance(); // singleton pattern

    /**
     * Initialize the non-blocking socket for listening socket. 
     */
    void InitializeSocket(PeerList& outPeerList);

    /**
     * Process socket events
     * 1. process non-blocking accept (and make recv_sockets)
     * 2. process non-blocking connect (and make send_sockets)
     * 3. process non-blocking recv
     */
    void ProcessNonblockSocket(PeerList& inPeerList, PeerList& outPeerList);

    void SendNetworkMsg(CentralizedNetworkMessage& msg, std::string dest);
};

std::string GetSerializedString(CentralizedNetworkMessage msg);
CentralizedNetworkMessage  GetDeserializedMsg(std::string str);




#endif // SIMPLE_SOCKET_INTERFACE_H
