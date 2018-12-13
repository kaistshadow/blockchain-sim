#ifndef HANDLE_NETWORK_TEST_H
#define HANDLE_NETWORK_TEST_H

#include "HandleNetwork.h"
#include "TestMessage.h"
#include "../datamodules/Peer.h"

#include <ev.h>
#include <string.h>
#include <map>

#define MYPORT 3456    /* the port users will be connecting to */
#define BACKLOG 10     /* how many pending connections queue will hold */

struct sock_ev {
  ev_io io;
  int fd;
};

class HandleNetworkTest: public HandleNetwork {
 private:
    PeerList membershipPeerList;
    PeerList gossipPeerList;

    // Map structure which maps socketfd into pointer of the Peer.
    // For a convenient access of the peer object.
    std::map< int, Peer* > peerMap;
    
    /* int serverListenSocket = -1; */
    int InitializeListenSocket();
    int ConnectToNode(std::string nodename);
    void RegisterServerWatcher(int listenfd);
    void RegisterSocketWatcher(int sfd); 

    std::string GetSerializedString(TestMessage& msg);
    TestMessage GetDeserializedMsg(std::string str);

 public:
    HandleNetworkTest() {}
    virtual ~HandleNetworkTest() {}

    int JoinNetwork();
    void HandleRecvSocketIO(int fd);
    void HandleSendSocketIO(int fd);
    void HandleAcceptSocketIO(int fd);

    void UnicastMsg(std::string destip, TestMessage& msg);
};



#endif // HANDLE_NETWORK_TEST_H
