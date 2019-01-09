#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <ev++.h>  // libev c++ api
#include <memory>

#include "EventCoordinator.h"
#include "EventInfo.h"


// basic template for publishing events
class Publisher {
 private:
    EventCoordinator *eventCoordinator;

 protected:
    void PublishEvent(std::shared_ptr<EventInfo> eventInfo) { eventCoordinator->PublishEvent(eventInfo); };

 public:
    Publisher(EventCoordinator* c): eventCoordinator(c) {}

};


// publisher for miningCompleteEvent
// it emulates the time duration of the mining with gaussian distribution
class MiningEventEmulator : public Publisher {
 private:
    double mining_avg_time;  // Configuration namespace needed!!
    double mining_avg_time_dev;
    ev::timer mining_timer;
    void onTimerEvent(ev::timer &w, int revents); // This private timer event will publish the explicit event by calling PublishEvent function


 public:
    MiningEventEmulator(EventCoordinator* c, double avg, double stddev): 
    Publisher(c), mining_avg_time(avg), mining_avg_time_dev(stddev), mining_timer(EV_DEFAULT) 
    { 
        mining_timer.set<MiningEventEmulator, &MiningEventEmulator::onTimerEvent> (this);
        // check http://pod.tst.eu/http://cvs.schmorp.de/libev/ev.pod#C_API-2
    }
    
    void StartMiningTimer();
    void StopMiningTimer();
};

#endif
