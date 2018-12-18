#ifndef HANDLE_NETWORK_PROXY_H
#define HANDLE_NETWORK_PROXY_H

#include "HandleNetwork.h"
#include "Message.h"
#include "../datamodules/Peer.h"

#include <ev.h>
#include <string.h>
#include <map>
#include <algorithm>

#include <list>
#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/list.hpp>


#define MYPORT 3456    /* the port users will be connecting to */
#define BACKLOG 10     /* how many pending connections queue will hold */


#define BROADCASTTYPE_UNICAST 0
#define BROADCASTTYPE_BROADCAST 1

class MessageHeader {
 private:
    int type;
    std::string destIP;
    std::list<std::string> visitedIPList;
    int message_len;
    
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
        ar & type;
        ar & destIP;
        ar & visitedIPList;
        ar & message_len;
    }

 public:
    MessageHeader() {}
    void SetBroadcastType(int t)  { type = t; }
    void SetDestIP(std::string ip) { destIP = ip; }
    void AppendVisitedIP(std::string ip) { visitedIPList.push_back(ip); }
    bool IsVisitedIP(std::string ip) { return (std::find(visitedIPList.begin(), visitedIPList.end(), ip) != visitedIPList.end()); }
    void SetMessageLength(int len) { message_len = len; }

    int GetBroadcastType() { return type; }
    std::string GetDestIP() { return destIP; }
    int GetMessageLength() { return message_len; }
};

class HandleNetwork_ProxyModel: public HandleNetwork {
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

    void RelayUnicastMsg(MessageHeader* header, Message* msg);
    void RelayBroadcastMsg(MessageHeader* header, Message* msg);

    std::string GetSerializedString(Message* msg);
    Message *GetDeserializedMsg(std::string str);

    std::string GetSerializedString(MessageHeader* header);
    MessageHeader *GetDeserializedMsgHeader(std::string str);

 public:
    HandleNetwork_ProxyModel() {}
    virtual ~HandleNetwork_ProxyModel() {}

    int JoinNetwork();
    void HandleRecvSocketIO(int fd);
    void HandleSendSocketIO(int fd);
    void HandleAcceptSocketIO(int fd);

    void UnicastMsg(std::string destip, Message* msg);
    void BroadcastMsg(Message* msg);
};



#endif // HANDLE_NETWORK_PROXY_H
