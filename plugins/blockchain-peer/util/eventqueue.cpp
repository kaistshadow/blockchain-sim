#include <iostream>
#include "eventqueue.h"


bool Event::IsEnd() {
    // check whether the event have awaited enough time.
    std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(current - register_time);
    if (time_elapsed.count() > ((double)duration_ms / 1000) ) 
        return true;
    else
        return false;
}

void Event::DoEvent() {
    event_func();
}

EventQueueManager* EventQueueManager::instance = 0;

EventQueueManager* EventQueueManager::GetInstance() {
    if (instance == 0) {
        instance = new EventQueueManager();
    }
    return instance;
}

void EventQueueManager::ProcessQueue() {
    std::list<Event>::iterator it = eventQueue.begin();
    while (it != eventQueue.end()) {
        Event& e = *it;
        if (e.IsEnd()) {
            e.DoEvent();
            eventQueue.erase(it++);
        }
        else {
            ++it;
        }
    }
}
