#ifndef EVENT_COORDINATOR_H
#define EVENT_COORDINATOR_H

#include <map>
#include <boost/signals2/signal.hpp>

#include "EventType.h"
#include "EventInfo.h"

typedef boost::signals2::signal<void (std::shared_ptr<EventInfo>) > Signal;
typedef std::function<void(std::shared_ptr<EventInfo> ) > Callback;

class EventCoordinator {
 private:
    EventCoordinator() {}; // singleton pattern
    static EventCoordinator* instance; // singleton pattern

    std::map< EventType, Signal > signalMap;
    // check https://stackoverflow.com/a/27373975 for understanding the signalMap structure
    // my example code https://ideone.com/Gy8A3f

 public:
    static EventCoordinator* GetInstance(); // sigleton pattern

    void PublishEvent(std::shared_ptr<EventInfo>);
    void SubscribeEvent(EventType type, Callback callback);

};




#endif
