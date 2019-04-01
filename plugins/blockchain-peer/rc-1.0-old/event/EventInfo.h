#ifndef EVENTINFO_H
#define EVENTINFO_H

#include "EventType.h"

class EventInfo {
 private:
    EventType type;
 public:
    EventInfo(EventType t = EventType::nullEvent): type(t) { }
    
    EventType& GetType() { return type; }
};

class MiningCompleteEventInfo : public EventInfo {
 private:
    int nonce;
 public:
    MiningCompleteEventInfo(int n) : EventInfo(EventType::miningCompleteEvent), nonce(n) { }
    int GetNonce() { return nonce; }
};

class RecvSocketConnectionEventInfo : public EventInfo {
 private:
    int listenfd;
 public:
    RecvSocketConnectionEventInfo(int fd) : EventInfo(EventType::recvSocketConnectionEvent), listenfd(fd) { }
    int GetListenFD() { return listenfd; }
};

class RecvSocketDataEventInfo : public EventInfo {
 private:
    int socketfd;
 public:
    RecvSocketDataEventInfo(int fd) : EventInfo(EventType::recvSocketDataEvent), socketfd(fd) { }
    int GetSocketFD() { return socketfd; }
};

class SendSocketReadyEventInfo : public EventInfo {
 private:
    int socketfd;
 public:
    SendSocketReadyEventInfo(int fd) : EventInfo(EventType::sendSocketReadyEvent), socketfd(fd) { }
    int GetSocketFD() { return socketfd; }
};

#endif
