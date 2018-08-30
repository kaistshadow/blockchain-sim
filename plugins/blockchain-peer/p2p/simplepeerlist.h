#ifndef P2P_SIMPLEPEERLIST_H
#define P2P_SIMPLEPEERLIST_H

#include <vector>
#include <string>

enum CONNECT_STATUS {
    IDLE = 0,
    CONNECTING = 1,
    CONNECTED = 2,
    NONE = 4,
};

enum RECV_STATUS {
    RECV_IDLE = 0,
    RECV_LENGTH = 1,
    RECV_NONE = 2,
};

class Peer{ 
 public:
    Peer(int socket) { sfd = socket; }
    Peer(std::string hn) { hostname = hn; conn_status = IDLE; }
    Peer(std::string hn, int socket) { hostname = hn; sfd = socket; }
    std::string hostname;
    int sfd;
    CONNECT_STATUS conn_status;
    RECV_STATUS recv_status;
    int payload_len;
};


typedef std::vector<Peer*> PeerList;

class SimplePeerList {
 private:
    SimplePeerList() { 
        inPeerList = std::vector<Peer*>();
        outPeerList = std::vector<Peer*>();
    }; // singleton pattern
    static SimplePeerList* instance; // singleton pattern
    
    PeerList inPeerList;
    PeerList outPeerList;

 public:
    static SimplePeerList* GetInstance(); // sigleton pattern

    /**
     * Initialize the list of neighbor nodes
     * This function will update the global variable called "peerList".
     */
    void AddPeerList(char *servhostname);
    
    PeerList& GetOutPeerList() { return outPeerList; };
    PeerList& GetInPeerList() { return inPeerList; };
};













#endif // P2P_SIMPLEPEERLIST_H
