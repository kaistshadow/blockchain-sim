#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <ev.h>
#include <memory>
#include <functional>

#include "EventCoordinator.h"
#include "EventType.h"
#include "EventInfo.h"

// Template for subscribing events
class Subscriber {
 private:
    EventCoordinator *eventCoordinator;
    Callback callback;
    EventType eventType;

 public:
    Subscriber(EventCoordinator* c, Callback cb, EventType t) : eventCoordinator(c), callback(cb), eventType(t) {
        eventCoordinator->SubscribeEvent(eventType, callback);
    }
};


#endif
