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

enum RECV_STATUS {
    RECV_IDLE = 0,
    RECV_HEADER = 1,
    RECV_MSG = 2,
    RECV_NONE = 3,
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

class SocketSendBuffer {
 public:
    std::list<std::shared_ptr<WriteMsg> > sendMsgQueue;
};

class SocketRecvBuffer {
 public:
    RECV_STATUS recv_status = RECV_IDLE;
    int message_len;
    int received_len;
    std::string recv_str;
    MessageHeader* header;   //  TODO: make MessageHeader a general interface class.
};


class HandleNetwork_ProxyModel: public HandleNetwork {
 private:
    PeerList membershipPeerList;
    PeerList gossipPeerList;

    // Map structure which maps socketfd into pointer of the Peer.
    // For a convenient access of the peer object.
    std::map< int, Peer* > peerMap;

    // Map structure which maps socketfd into the SocketSendBuffer
    std::map< int, SocketSendBuffer > sendBuffMap;
    // Map structure which maps socketfd into the SocketRecvBuffer
    std::map< int, SocketRecvBuffer > recvBuffMap;


    /* int serverListenSocket = -1; */
    int InitializeListenSocket();
    int ConnectToNode(std::string nodename);

    void RelayUnicastMsg(MessageHeader* header, Message* msg);
    void RelayBroadcastMsg(MessageHeader* header, Message* msg);

    std::string GetSerializedString(Message* msg);
    Message *GetDeserializedMsg(std::string str);

    std::string GetSerializedString(MessageHeader* header);
    MessageHeader *GetDeserializedMsgHeader(std::string str);

    // for testing
    void PrintGossipPeerList();

 public:
    HandleNetwork_ProxyModel() {}
    ~HandleNetwork_ProxyModel() {}

    int JoinNetwork();

    void UnicastMsg(std::string destip, Message* msg);
    void BroadcastMsg(Message* msg);

    // Event subscriber callback 1. for recvSocketConnectionEvent
    void onRecvSocketConnectionEvent(std::shared_ptr<EventInfo>);
    // Event subscriber callback 2. for recvSocketDataEvent
    void onRecvSocketDataEvent(std::shared_ptr<EventInfo>);
    // Event subscriber callback 3. for sendSocketReadyEvent
    void onSendSocketReadyEvent(std::shared_ptr<EventInfo>);


    //for testing
    void setcontact(std::string id){;}
};



#endif // HANDLE_NETWORK_PROXY_H
