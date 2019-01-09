#include "Publisher.h"
#include "../utility/GlobalClock.h"
#include "../utility/NodeInfo.h"

void MiningEventEmulator::onTimerEvent(ev::timer &w, int revents) {
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
