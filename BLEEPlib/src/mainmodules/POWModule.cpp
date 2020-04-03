#include "POWModule.h"
#include "../utility/Assert.h"
#include "../utility/Random.h"
#include "shadow_interface.h"

using namespace libBLEEP;

void POWModule::AsyncEmulateBlockMining(std::shared_ptr<POWBlock> candidateBlk, double avg, double stddev) {
    if (avg < 0) {
        M_Assert(0, "average value of the time should be positive value");
    }

    double waiting_time = get_global_random_source(GetHostNumber()).get_normal_value(avg, stddev, RAND_DROP_NEGATIVE);

    // std::cout << "waiting time = " << waiting_time << "\n";
    watcherManager.CreateMiningEmulationTimer(candidateBlk, waiting_time);

    // append shadow api log
    char buf[256];
    sprintf(buf, "API,AsyncEmulateBlockMining,%f,%f", avg, stddev);
    shadow_push_eventlog(buf);
}

void POWModule::AsyncBlockMining(std::shared_ptr<POWBlock> candidateBlk, UINT256_t difficulty) {
    watcherManager.CreateMiningThread(candidateBlk, difficulty);

    // append shadow api log
    char buf[256];
    sprintf(buf, "API,AsyncBlockMining");
    shadow_push_eventlog(buf);
}


void POWModule::StopMining() {
    watcherManager.StopMiningThread();
    watcherManager.RemoveMiningEmulationTimer();    
    _isMining = false;

    // append shadow api log
    char buf[256];
    sprintf(buf, "API,StopMining");
    shadow_push_eventlog(buf);
}
