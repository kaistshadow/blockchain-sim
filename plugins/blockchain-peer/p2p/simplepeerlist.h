#ifndef P2P_SIMPLEPEERLIST_H
#define P2P_SIMPLEPEERLIST_H

#include <vector>
#include <string>

enum CONNECT_STATUS {
    IDLE = 0,
    CONNECTING = 1,
    CONNECTED = 2,
};

class Peer{ 
 public:
    Peer(std::string hn) { hostname = hn; conn_status = IDLE; }
    Peer(std::string hn, int socket) { hostname = hn; sfd = socket; }
    std::string hostname;
    int sfd;
    CONNECT_STATUS conn_status;
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
    void InitializePeerList(char *servhostname);
    
    PeerList GetOutPeerList() { return outPeerList; };

};













#endif // P2P_SIMPLEPEERLIST_H
