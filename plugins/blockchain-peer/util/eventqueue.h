#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <list>
#include <chrono>
#include <functional>

class Event {
 public:
    Event(int millisecond, std::function<void ()> func) { register_time = std::chrono::high_resolution_clock::now(); duration_ms = millisecond; event_func = func; };
    bool IsEnd();
    void DoEvent();
 private:
    std::chrono::high_resolution_clock::time_point register_time;
    int duration_ms;
    std::function<void ()> event_func;
};

class EventQueueManager {
 private:
    EventQueueManager() {} // singleton pattern
    static EventQueueManager* instance; // singleton pattern

    std::list<Event> eventQueue;
 public:
    static EventQueueManager* GetInstance(); // singleton pattern
    
    void PushEvent(Event e) { eventQueue.push_back(e); }
    void ProcessQueue();

};


#endif
