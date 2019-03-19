#ifndef PEER_H
#define PEER_H

#include <list>
#include <string>
#include <string.h>

class MessageHeader;

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

enum RECV_STATUS {
    RECV_IDLE = 0,
    RECV_HEADER = 1,
    RECV_MSG = 2,
    RECV_NONE = 3,
};

// SocketEventStatus : data structure that contains any status for socket communication
class SocketEventStatus {
 public:
    RECV_STATUS recv_status = RECV_IDLE;
    int message_len;
    int received_len;
    std::string recv_str;
    MessageHeader* header;   //  TODO: make MessageHeader a general interface class.

    std::list<WriteMsg *> sendMsgQueue;
};

class Peer{ 
 private:
    std::string hostname;
    std::string ipaddr;
    int socketfd = -1;
    
    SocketEventStatus status;

 public:
    Peer() {}
    Peer(int fd, std::string ip) { socketfd = fd; ipaddr = ip; }
    Peer(int fd, std::string domain, std::string ip) { socketfd = fd; hostname = domain; ipaddr = ip; }
    std::string GetHostname() { return hostname; }
    std::string GetIP() { return ipaddr; }
    int GetSocketFD() { return socketfd; }
    SocketEventStatus& GetSocketEventStatus() { return status; }

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
