#ifndef HANDLE_NETWORK_GOSSIP_H
#define HANDLE_NETWORK_GOSSIP_H

#include "HandleNetwork.h"
#include "Message.h"
#include "../datamodules/Peer.h"

#include <ev.h>
#include <string.h>
#include <map>
#include <list>
#include <vector>
#include <utility>
#include <algorithm>
#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>

#define MYPORT 3456    /* the port users will be connecting to */
#define BACKLOG 10     /* how many pending connections queue will hold */

// [Membership Protocol Parameters]
// scale 10000: (5,30,6,3,5,3,4)
// scale 1000 : (4,24,5,2,4,2,3)
// scale 100  : (3,18,4,2,3,2,3)
#define ActiveViewSize  4
#define PassiveViewSize 24
#define ARWL 5
#define PRWL 2
#define SRWL 4
#define Ka   2
#define Kp   3
#define SHUFFLE_OP 1
#define SHUFFLE_PERIOD 5
#define CONNECTION_TIMEOUT 12000
#define TEST_OP 0

// [Gossip Protocol Parameters]
#define STORAGE_SIZE     100
#define RECOVERY_TIMEOUT 20

// [type_cast of GossipModuleHeader]
#define CASTTYPE_UNICAST   1
#define CASTTYPE_BROADCAST 2

// [type_protocol of GossipModuleHeader]
#define PROTOCOL_MEMBERSHIP 1
#define PROTOCOL_GOSSIP     2
#define PROTOCOL_CONSENSUS  3

// [type of GossipModuleMessage]
#define MEMBERSHIP_JOIN             1
#define MEMBERSHIP_FORWARDJOIN      2
#define MEMBERSHIP_FORWARDJOINREPLY 3
#define MEMBERSHIP_DISCONNECT       4
#define MEMBERSHIP_NEIGHBOR         5
#define MEMBERSHIP_NEIGHBORREPLY    6
#define MEMBERSHIP_SHUFFLE          7
#define MEMBERSHIP_SHUFFLEREPLY     8
#define GOSSIP_GOSSIP 1
#define GOSSIP_IHAVE  2
#define GOSSIP_GRAFT  3
#define GOSSIP_PRUNE  4

class GossipModuleHeader;

/* [Socket Management related classes/structures]
 * These data structures are used for managing socket's status, buffers.
 */
enum STATUS {
    STATUS_IDLE   = 0,
    STATUS_HEADER = 1,
    STATUS_MSG    = 2,
    STATUS_NONE   = 3
};

struct SendMsg {
    char*  data;
    ssize_t len;
    ssize_t pos;

    SendMsg (const char *bytes, ssize_t nbytes) {
        pos  = 0;
        len  = nbytes;
        data = new char[nbytes];
        memcpy(data, bytes, nbytes);
    }
    virtual ~SendMsg () {delete [] data;}
    char*   dpos()   {return data + pos;}
    ssize_t nbytes() {return len - pos;}
};

class SocketSendStatus {
 public:
    std::list< std::shared_ptr<SendMsg> > sendMsgQueue;
};

class SocketRecvStatus {
 public:
    STATUS recv_status = STATUS_IDLE;
    int message_len;
    int received_len;
    std::string recv_str;
    GossipModuleHeader* header;
};

/* [GossipModuleHeader]
 * This is header class of all messages and is specialized at Gossip module
 * And,like GossipModuleMessage, only used at this Gossip Module
 */
class GossipModuleHeader {
 private:
    int type;
    int type_protocol;
    int hop;
    int message_len;
    size_t message_id;
    std::string dstId;
    std::string srcId;
    bool is_injected;

    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    template<class Archive> void serialize(Archive & ar, const unsigned int version) {
        ar & type;
        ar & type_protocol;
        ar & hop;
        ar & message_len;
        ar & message_id;
        ar & dstId;
        ar & srcId;
        ar & is_injected;
    }

 public:
    GossipModuleHeader() {}
    GossipModuleHeader(int transfertype, int protocoltype): type(transfertype), type_protocol(protocoltype) {
        hop = 0;
        message_len = -1;
        message_id  = 0;
        is_injected = false;
    }

    void SetTransferType(int t)    {type = t;}
    void SetProtocolType(int t)    {type_protocol = t;}
    void SetHopCount(int n)        {hop = n;}
    void IncreaseHopCount()        {hop += 1;}
    void SetMessageLength(int len) {message_len = len;}
    void SetMessageId(size_t id)   {message_id = id;}
    void SetDstId(std::string id)  {dstId = id;}
    void SetSrcId(std::string id)  {srcId = id;}
    void SetIsInjected(bool t)     {is_injected = t;}

    int GetTransferType()  {return type;}
    int GetProtocolType()  {return type_protocol;}
    int GetHopCount()      {return hop;}
    int GetMessageLength() {return message_len;}
    size_t GetMessageId()  {return message_id;}
    std::string GetDstId() {return dstId;}
    std::string GetSrcId() {return srcId;}
    bool IsInjected()      {return is_injected;}
};

/* [GossipModuleMessage]
 * Message subclass used at Gossip module only
 * This is for two protocol, HyParView and PlumTree
 * type and option for both, but shufflelist is only for HyParView
 */
class GossipModuleMessage : public Message {
 private:
    int type;
    size_t option;
    std::vector<std::string> shufflelist; //only for membership protocol

    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    template<class Archive> void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Message>(*this);
        boost::serialization::void_cast_register<Message, GossipModuleMessage>();
        ar & type;
        ar & option;
        ar & shufflelist;
    }

 public:
    GossipModuleMessage() {}
    GossipModuleMessage(int t): type(t) {}
    virtual int GetType() {return GOSSIPMODULE_MESSAGE;}
    virtual ~GossipModuleMessage() {}

    void SetOption(size_t opt) {option = opt;}
    void SetShuffleList(std::vector<std::string> slist) {shufflelist = slist;}

    int GetMessageType() {return type;}
    size_t GetOption()   {return option;}
    std::vector<std::string> GetShuffleList() {return shufflelist;}
};

/* [TimeoutEntry] & [TimeoutManager]
 * @@ "setsockopt" function call + "SO_RCVTIMEOUT" option not supported
 * Classes for implementing connection IDLE timeout to reduce TCP connection cost
 * If a connection does not receive recv/send signal for CONNECTION_TIMEOUT(seconds),
 * Then remove all classes/structures related to the connection and close the socket.
 */
class TimeoutEntry {
 public:
    TimeoutEntry(int newsfd): sfd(newsfd) {}
    int sfd;
    ev_timer timeout_timer;
};

class TimeoutList {
 private:
    std::vector<TimeoutEntry*> timeoutlist;
    TimeoutEntry* FindTimeoutEntry(int sfd);

 public:
    TimeoutList() {}
    void RegisterTimeoutWatcher(int sfd);
    void RestartTimer(int sfd);
    void RemoveTimer(int sfd);
    static void ConnectionTimeoutCallback(EV_P_ ev_timer *w, int revents);
};

/* [Neighbor] & [PartialView]
 * Classes for managing partial views of Network used by Membership & Gossip Protocol.
 * id is for both protocol, but isEager is only for Gossip protocol.
 * Addtion and Deletion are Membership parts, isEager state change is Gossip part.
 */
class Neighbor {
 public:
    Neighbor(std::string newid): id(newid), isEager(true) {}
    std::string id;
    bool isEager;
};

class PartialView {
 private:
    std::vector<Neighbor*> active;
    std::vector<Neighbor*> passive;

 public:
    PartialView() {}
    std::vector<Neighbor*>& GetActiveView()  {return active;}
    std::vector<Neighbor*>& GetPassiveView() {return passive;}

    int CurrentActiveViewSize() {return active.size();}
    void AddToActive(std::string id);
    void DropFromActive(std::string id);
    void DropRandomFromActive();
    Neighbor* FindPeerFromActive(std::string id);
    Neighbor* ChooseRandomFromActive();
    Neighbor* ChooseRandomFromActiveExcept(std::string sender);

    int CurrentPassiveViewSize() {return passive.size();}
    void AddToPassive(std::string id);
    void DropFromPassive(std::string id);
    void DropRandomFromPassive();
    Neighbor* FindPeerFromPassive(std::string id);
    Neighbor* ChooseRandomFromPassive();
    Neighbor* ChooseRandomFromPassiveExcept(std::string sender);

    std::vector<std::string> GetRandomActiveMemberList(int n);
    std::vector<std::string> GetRandomPassiveMemberList(int n);

    void PrintActiveView();
    void PrintPassiveView();
};

/* [IHAVE_MESSAGE] & [MessageStorage]
 * This classes store consensus level msgs and are parts of Gossip protocol
 * (only care about CONSENSUS level's BROADCAST messages, not UNICAST.)
 * These combine message id(mid) and original msg first, then store it into storage.
 * Storage size is specified with STORAGE_SIZE parameter and this means it has a limitation.
 * TODO : change memory storage to file sys storage to store all messages inside the network.
 */
class IHAVE_MESSAGE {
 public:
    IHAVE_MESSAGE(size_t mID, int Round, std::string Sender, Message* Msg): mid(mID), round(Round), sender(Sender), msg(Msg) {}
    size_t mid;
    int  round;
    std::string sender;
    Message* msg;
};

class MessageStorage {
 private:
    std::vector<IHAVE_MESSAGE> storage;
    std::string GetSerializedMsg(Message* msg);

 public:
    MessageStorage() {}
    bool IsExist(size_t mid);
    void AddToStorage(size_t mid, int round, std::string sender, Message* msg);
    size_t CreateNewId(Message* msg);
    IHAVE_MESSAGE* GetMessageEntry(size_t mid);
};

/* [RecoveryEntry] & [RecoveryStorage]
 * Classes for Gossip protocol's recovery part which IHAVE and GRAFT are related to.
 * Use these classes to restore and fix EAGER link, drop problematic one and use new one.
 * recovery_timers are allocated to every link and control/decide their link's state.
 */
class RecoveryEntry {
 public:
    RecoveryEntry(size_t mID, int Round, std::string Sender): mid(mID), round(Round), sender(Sender) {
        timer_phase = 0;
    }
    size_t mid;
    int  round;
    std::string sender;
    int timer_phase;
    ev_timer recovery_timer;
};

class RecoveryStorage {
 private:
    std::map< size_t, std::vector<RecoveryEntry*> > storage;
    void StartTimer(size_t mid);

 public:
    RecoveryStorage() {}
    void AddToStorage(size_t mid, int round, std::string sender);
    bool StopTimerAndRemoveEntries(size_t mid);
    static void RecoveryTimeoutCallback(EV_P_ ev_timer *w, int revents);
    RecoveryEntry* FindFirstRecoveryEntry(size_t mid);
};

/* [HandleNetwork_GossipModel] */
class HandleNetwork_GossipModel: public HandleNetwork {
 private:
    static HandleNetwork_GossipModel* instance;

    std::map< int, Peer* > PeerMap;                  // sfd-Peer mapping
    std::map< int, SocketSendStatus > sendStatusMap; // sfd-SocketSendBuffer mapping
    std::map< int, SocketRecvStatus > recvStatusMap; // sfd-SocketRecvBuffer maaping

    TimeoutList     TimeoutManager;           // manage all connections' timeout
    PartialView     PartialViewManager;       // GOSSIP : manage two partial views
    MessageStorage  Gossip_MessageManager;    // GOSSIP : store consensus-broadcast message
    RecoveryStorage Gossip_RecoveryManager;   // GOSSIP : store ihave info and manage timeout
    ev_timer        Membership_CyclonTimer;   // MEMBERSHIP : for periodic shuffle sending
    ev_timer        Membership_ShutdownTimer; // MEMBERSHIP : for shutdown

    // [Functions for Socket Event Management]
    // 0. Inintiailize server-role socket descriptor
    // 1. Event subscriber callback for recvSocketConnectionEvent
    // 2. Event subscriber callback for recvSocketDataEvent
    // 3. Event subscriber callback for sendSocketReadyEvent
    int  InitializeListenSocket();
    void onRecvSocketConnectionEvent(std::shared_ptr<EventInfo>);
    void onRecvSocketDataEvent(std::shared_ptr<EventInfo>);
    void onSendSocketReadyEvent(std::shared_ptr<EventInfo>);

    // [Functions for P2P networking protocols]
    Peer* GetPeerFromPeerMap(std::string id);
    std::string GetSerializedMsg(Message* msg);
    Message*    GetDeserializedMsg(std::string str);
    std::string GetSerializedMsgHeader(GossipModuleHeader* header);
    GossipModuleHeader* GetDeserializedMsgHeader(std::string str);

    static void Membership_HandleShutdownCallback(EV_P_ ev_timer *w, int revents);
    static void Membership_HandleSendShuffleCallback(EV_P_ ev_timer *w, int revents);
    void Membership_RegisterShutdownTimerWatcher();
    void Membership_RegisterShuffleTimerWatcher();
    int  Membership_ProcessShuffleList(std::vector<std::string> list);
    void Membership_BroadcastWithHeader(std::string exception, GossipModuleHeader* header, Message* msg);
    void Membership_SendNeighbor(std::string exception);
    void Membership_HandleCrush(std::string src);
    void Membership_RunProtocol(std::string sender, GossipModuleHeader* header, Message* msg);

    void Gossip_BroadcastWithHeader(std::string exception, GossipModuleHeader* header, Message* msg, bool IsDstEager);
    void Gossip_LazyPush(size_t mid, int round);
    void Gossip_HandleCrush(std::string src);
    void Gossip_RunProtocol(std::string sender, GossipModuleHeader* header, Message* msg);

 public:
    HandleNetwork_GossipModel() {}
    ~HandleNetwork_GossipModel() {}
    static HandleNetwork_GossipModel* GetInstance();

    std::map<int, Peer*>* GetPeerMap()   {return &PeerMap;}
    TimeoutList* GetTimeoutManager()     {return &TimeoutManager;}
    PartialView* GetPartialViewManager() {return &PartialViewManager;}

    std::vector<std::string> Membership_CreateShuffleList(int na, int np);
    void Membership_HandleDisconnect(Peer* peer);
    int  Connect(std::string targetid);
    int  UnicastWithHeader(GossipModuleHeader* header, Message* msg);

    /* APIs */
    int  JoinNetwork();
    void UnicastMsg(std::string destid, Message* msg);
    void BroadcastMsg(Message* msg);
};

#endif // HANDLE_NETWORK_GOSSIP_H
