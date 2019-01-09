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

#endif
