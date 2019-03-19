#ifndef PEERLIST_MANAGER_COMBINED_H
#define PEERLIST_MANAGER_COMBINED_H

#include <vector>
#include <string>

struct sock_ev_receiver;

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

class PeerCombined{ 
 public:
    PeerCombined() {}
    PeerCombined(std::string domain, std::string ip) { hostname = domain; ipaddr = ip; }
    std::string hostname;
    std::string ipaddr;


    int receiver_sfd;
    RECV_STATUS recv_status;
    int payload_len;
    int received_len;
    std::string recv_str;
    void SetReceiverSocket(int socket) { receiver_sfd = socket; }

    sock_ev_receiver *ev_receiver = nullptr; // structure for ev_io
};


typedef std::vector<PeerCombined*> PeerList;

class PeerListManagerCombined {
 private:
    PeerListManagerCombined() { 
        peerList = std::vector<PeerCombined*>();
    }; // singleton pattern
    static PeerListManagerCombined* instance; // singleton pattern
    
    PeerList peerList;
    std::string myHostId;
    
 public:
    static PeerListManagerCombined* GetInstance(); // sigleton pattern

    /* void InitializeDomainList(int num, char *hostnames[]); */

    /**
     * Initialize the list of neighbor nodes
     * This function will update the global variable called "peerList".
     */
    void InitializeMyPeer(std::string hn) { myHostId = hn; }
    std::string GetHostId() { return myHostId; }
    
    PeerList& GetPeerList() { return peerList; };
    /* PeerList& GetOutPeerList() { return outPeerList; }; */
    /* PeerList& GetInPeerList() { return inPeerList; }; */

    /* std::string GetDomainFromIp(std::string ip); */
    /* void UpdateDomainNameForIp(std::string ip, std::string domain); */

    PeerCombined* GetPeerByDomain(std::string domain);
};













#endif // PEERLIST_MANAGER_H
