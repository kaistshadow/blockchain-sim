#ifndef PEERLIST_MANAGER_H
#define PEERLIST_MANAGER_H

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
    std::string ipaddr;
    int sfd;
    CONNECT_STATUS conn_status;
    RECV_STATUS recv_status;
    int payload_len;
    int received_len;
    std::string recv_str;
};


typedef std::vector<Peer*> PeerList;

class SimplePeerListManager {
 private:
    SimplePeerListManager() { 
        inPeerList = std::vector<Peer*>();
        outPeerList = std::vector<Peer*>();
    }; // singleton pattern
    static SimplePeerListManager* instance; // singleton pattern
    
    PeerList inPeerList;
    PeerList outPeerList;
    std::string myHostId;

 public:
    static SimplePeerListManager* GetInstance(); // sigleton pattern

    /**
     * Initialize the list of neighbor nodes
     * This function will update the global variable called "peerList".
     */
    void InitializeOutPeerList(int num, char *hostname_outpeers[]);
    
    void InitializeMyPeer(std::string hn) { myHostId = hn; }
    std::string GetHostId() { return myHostId; }
    
    PeerList& GetOutPeerList() { return outPeerList; };
    PeerList& GetInPeerList() { return inPeerList; };

    std::string GetDomainFromIp(std::string ip);
    void UpdateDomainNameForIp(std::string ip, std::string domain);
};













#endif // PEERLIST_MANAGER_H
