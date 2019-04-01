#include "Publisher.h"
#include "../utility/GlobalClock.h"
#include "../utility/NodeInfo.h"

void MiningEventEmulator::timerCallback(ev::timer &w, int revents) {
    std::shared_ptr<EventInfo> info = std::shared_ptr<EventInfo>(new MiningCompleteEventInfo(0)); // since this is an emulator, we use arbitrary nonce value.

    PublishEvent(info);
}

void MiningEventEmulator::StartMiningTimer() {
    // trigger a new mining timer
    // by calculating the emulated mining time
    double waiting_time = 0;

    while (waiting_time <= 0) {
        unsigned int random_num = time(0) * NodeInfo::GetInstance()->GetHostNumber();
        std::default_random_engine generator(random_num);
        std::normal_distribution<double> distribution(mining_avg_time, mining_avg_time_dev);
        waiting_time = distribution(generator);
    }

    double emulated_mining_time = utility::GetGlobalClock() + waiting_time;
    std::cout << utility::GetGlobalClock() << ":Set next emulated mining time:" << emulated_mining_time << "\n";
    // state = MiningEventWaiting;

    // register mining timer
    mining_timer.set(waiting_time, 0.);
    mining_timer.start();
    // ev_timer_set (&mining_timer, waiting_time, 0.);
    // ev_timer_start (GlobalEvent::loop, &mining_timer);
}

void MiningEventEmulator::StopMiningTimer() {
    mining_timer.stop();
    // ev_timer_stop(GlobalEvent::loop, &mining_timer);
}

void SocketEventPublisher::listenSocketIOCallback(ev::io &w, int revents) {
    std::shared_ptr<EventInfo> info = std::shared_ptr<EventInfo>(new RecvSocketConnectionEventInfo(w.fd)); 

    PublishEvent(info);
}

void SocketEventPublisher::dataSocketIOCallback(ev::io &w, int revents) {
    if (revents & EV_READ) {
        std::shared_ptr<EventInfo> info = std::shared_ptr<EventInfo>(new RecvSocketDataEventInfo(w.fd)); 
        PublishEvent(info);
    } 
    if (revents & EV_WRITE) {
        std::shared_ptr<EventInfo> info = std::shared_ptr<EventInfo>(new SendSocketReadyEventInfo(w.fd)); 
        PublishEvent(info);
    }
}

void SocketEventPublisher::RegisterSocketAsServerSocket(int fd) {
    listenSocketWatchers.emplace_back();
    ev::io& io_watcher = listenSocketWatchers.back();
    io_watcher.set<SocketEventPublisher, &SocketEventPublisher::listenSocketIOCallback> (this);
    io_watcher.set(fd, ev::READ);
    io_watcher.start();
}

void SocketEventPublisher::RegisterSocketAsDataSocket(int fd) {
    dataSocketWatchers.emplace_back();
    ev::io& io_watcher = dataSocketWatchers.back();
    io_watcher.set<SocketEventPublisher, &SocketEventPublisher::dataSocketIOCallback> (this);
    io_watcher.set(fd, ev::READ);
    io_watcher.start();
    dataSocketMap[fd] = &io_watcher;
}

void SocketEventPublisher::UnregisterDataSocket(int fd) {
    dataSocketWatchers.remove_if([fd](ev::io& w) { return fd == w.fd; });
    dataSocketMap.erase(fd);
}

void SocketEventPublisher::SetSocketWrite(int fd) {
    ev::io* io_watcher = dataSocketMap[fd];
    io_watcher->set(fd, ev::READ | ev::WRITE);  
    // std::cout << "SetSocketWrite for fd " << fd << "\n";
    // Unlike the C counterpart of libev, an active watcher gets automatically stopped and restarted when reconfiguring it with the set method. check http://pod.tst.eu/http://cvs.schmorp.de/libev/ev.pod#C_API-2
}

void SocketEventPublisher::UnsetSocketWrite(int fd) {
    ev::io* io_watcher = dataSocketMap[fd];
    io_watcher->set(fd, ev::READ);
}
