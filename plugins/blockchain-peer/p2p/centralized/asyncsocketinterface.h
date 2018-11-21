#ifndef ASYNC_SOCKET_INTERFACE_H
#define ASYNC_SOCKET_INTERFACE_H

#include <vector>
#include <queue>
#include <map>
#include <utility>

#include "peerlistmanager_combined.h"
#include "centralizednetworkmessage.h"
#include <ev.h>

#define MYPORT 3456    /* the port users will be connecting to */
#define BACKLOG 10     /* how many pending connections queue will hold */

using namespace std;

#define PROCESSINGQUEUETYPE_PROXY 0
#define PROCESSINGQUEUETYPE_NODE 1

struct sock_ev_serv {
  ev_io io;
  int fd;
};

struct sock_ev_receiver {
  ev_io io;
  int fd;
  PeerCombined* peer;
};

/* struct sock_ev_sender { */
/*   ev_io io; */
/*   int fd; */
/*   PeerCombined* peer; */
/* }; */

struct periodic_ev_connect {
    ev_periodic periodic;
    std::vector<std::pair<Domain, int> > domain_sfd_pairs;
};

struct WriteMsg {
    char       *data;
    ssize_t len;
    ssize_t pos;
  
    WriteMsg (const char *bytes, ssize_t nbytes) {
        pos = 0;
        len = nbytes;
        data = new char[nbytes];
        memcpy(data, bytes, nbytes);
    }
  
    virtual ~WriteMsg () {
        delete [] data;
    }
  
    char *dpos() {
        return data + pos;
    }
  
    ssize_t nbytes() {
        return len - pos;
    }
};

class AsyncSocketInterface {
 private:
    AsyncSocketInterface(){};  // singleton pattern
    static AsyncSocketInterface* instance; // singleton pattern

    void InitServerSocket();
    
    static void ReceiverCallback(EV_P_ ev_io *w, int revents);
    static void ServerCallback(EV_P_ ev_io *w, int revents);
    static void SenderCallback(EV_P_ ev_io *w, int revents);
    static void ConnectCallback(EV_P_ ev_periodic *w, int revents);

    struct ev_loop* loop;
    struct sock_ev_serv *server;

    struct ev_periodic periodic_connect;
    int server_sfd;

    std::map< std::string, std::list<WriteMsg *> > msgQueueMap;
    
    int processingQueueType;

 public:
    std::map<std::string, std::list<WriteMsg *> >& GetMsgQueueMap() { return msgQueueMap; }
    void SetProcessingQueueType(int type) { processingQueueType = type; }
    int GetProcessingQueueType() { return processingQueueType; }


    /**
     * Set and Get a server socket
     */
    void SetListenSocket(int sfd) { server_sfd = sfd; }
    int GetListenSocket() { return server_sfd; }


    static AsyncSocketInterface* GetInstance(); // singleton pattern

    /**
     * Initialize the non-blocking socket for listening socket. 
     */
    void InitializeListenSocket();


    void SetEvLoop(struct ev_loop* l);
    void RegisterServerWatcher();
    void RegisterPeriodicConnectWatcher(int num, char *domains[]);


    /**
     * Process socket events
     * 1. process non-blocking accept (and make recv_sockets)
     * 2. process non-blocking connect (and make send_sockets)
     * 3. process non-blocking recv
     */
    /* void ProcessNonblockSocket(PeerList& inPeerList, PeerList& outPeerList); */

    void SendNetworkMsg(CentralizedNetworkMessage& msg, std::string dest);
};

std::string GetSerializedString(CentralizedNetworkMessage msg);
CentralizedNetworkMessage  GetDeserializedMsg(std::string str);




#endif // SIMPLE_SOCKET_INTERFACE_H
