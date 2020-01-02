#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <ev++.h>  // libev c++ api
#include <memory>
#include <map>
#include <list> 
#include <iostream> 

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
    void timerCallback(ev::timer &w, int revents); // This private timer callback will publish the explicit event by calling PublishEvent function


 public:
    MiningEventEmulator(EventCoordinator* c, double avg, double stddev): 
    Publisher(c), mining_avg_time(avg), mining_avg_time_dev(stddev), mining_timer(EV_DEFAULT) 
    { 
        mining_timer.set<MiningEventEmulator, &MiningEventEmulator::timerCallback> (this);
        // check http://pod.tst.eu/http://cvs.schmorp.de/libev/ev.pod#C_API-2
    }
    
    void StartMiningTimer();
    void StopMiningTimer();
};

// publisher for recvSocketConnectionEvent
class SocketEventPublisher : public Publisher {
 private:
    std::list<ev::io> listenSocketWatchers;
    std::list<ev::io> dataSocketWatchers;
    
    std::map<int, ev::io*> dataSocketMap; // map fd -> watcher

    // Following private io callbacks will publish the explicit event by calling PublishEvent function
    void listenSocketIOCallback(ev::io &w, int revents); 
    void dataSocketIOCallback(ev::io &w, int revents);

 public:
    SocketEventPublisher(EventCoordinator* c = EventCoordinator::GetInstance()): Publisher(c) { }

    void RegisterSocketAsServerSocket(int fd); 
    // This function register listen socket to publisher.
    // After registration, the publisher will 'publish' recvSocketConnectionEvent whenever the connection is requested through the registered socket.

    void RegisterSocketAsDataSocket(int fd);
    // This function register data socket to publisher.
    // After registration, the publisher will 'publish' recvSocketDataEvent whenever the data is arrived at the registered socket.

    void UnregisterDataSocket(int fd);
    
    void SetSocketWrite(int fd);
    // This function set EV_WRITE flag for given socket watcher in order to reserve the write socket event.
    // Whenever the given socket is ready for write, sendSocketReadyEvent will be published.
    void UnsetSocketWrite(int fd);
    // This function unset EV_WRITE flag for given socket watcher.
    // After unset, sendSocketReadyEvent will not be published.

};

#endif
