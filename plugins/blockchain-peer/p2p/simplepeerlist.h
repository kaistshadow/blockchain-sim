#ifndef P2P_SIMPLEPEERLIST_H
#define P2P_SIMPLEPEERLIST_H

#include <vector>
#include <string>

class Peer{ 
 public:
    Peer(std::string hn, int socket) { hostname = hostname; sfd = socket;}
    std::string hostname;
    int sfd;
};

typedef std::vector<Peer> PeerList;

extern PeerList inPeerList;
extern PeerList outPeerList;

/**
 * Initialize the list of neighbor nodes
 * This function will update the global variable called "peerList".
 */
void InitializePeerList();




















#endif // P2P_SIMPLEPEERLIST_H
