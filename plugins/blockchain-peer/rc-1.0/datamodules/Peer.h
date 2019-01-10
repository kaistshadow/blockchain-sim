#ifndef PEER_H
#define PEER_H

#include <list>
#include <string>
#include <string.h>

class Peer{ 
 private:
    std::string hostname;
    std::string ipaddr;
    int socketfd = -1;
    

 public:
    Peer() {}
    Peer(int fd, std::string ip) { socketfd = fd; ipaddr = ip; }
    Peer(int fd, std::string domain, std::string ip) { socketfd = fd; hostname = domain; ipaddr = ip; }
    std::string GetHostname() { return hostname; }
    std::string GetIP() { return ipaddr; }
    int GetSocketFD() { return socketfd; }
};


class PeerList {
 private:
    std::list<Peer*> peerList;
 public:
    PeerList() { 
        peerList = std::list<Peer*>();
    }
    
    std::list<Peer*>& GetPeerList() { return peerList; }
    void AppendPeer(Peer* p) { peerList.push_back(p); }
    void RemovePeer(Peer* p) { peerList.remove(p); }
    /* std::string GetDomainFromIp(std::string ip); */
    /* void UpdateDomainNameForIp(std::string ip, std::string domain); */

    Peer* GetPeerByDomain(std::string domain);
    Peer* GetPeerByIP(std::string ipaddr);
};


#endif
