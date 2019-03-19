#include "EventCoordinator.h"

EventCoordinator* EventCoordinator::instance = 0;

EventCoordinator* EventCoordinator::GetInstance() {
    if (instance == 0) {
        instance = new EventCoordinator();
    }
    return instance;
}

void EventCoordinator::PublishEvent(std::shared_ptr<EventInfo> info) {
    EventType &type = info->GetType();
    Signal& sig = signalMap[type];
    sig(info);
}


void EventCoordinator::SubscribeEvent(EventType type, Callback callback) {
    signalMap[type].connect(callback);
}
