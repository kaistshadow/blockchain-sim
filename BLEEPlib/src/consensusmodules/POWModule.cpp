#include "POWModule.h"
#include "../utility/Assert.h"

using namespace libBLEEP;

void POWModule::AsyncEmulateBlockMining(std::shared_ptr<POWBlock> candidateBlk, double avg, double stddev) {
    if (avg < 0) {
        M_Assert(0, "average value of the time should be positive value");
    }

    double waiting_time = -1;
    while (waiting_time < 0) {
        unsigned int random_num = time(0);
        std::default_random_engine generator(random_num);
        std::normal_distribution<double> distribution(avg, stddev);
        waiting_time = distribution(generator);
    }

    std::cout << "waiting time = " << waiting_time << "\n";
    watcherManager.CreateMiningEmulationTimer(candidateBlk, waiting_time);
}

void POWModule::StopMining() {
    watcherManager.RemoveMiningEmulationTimer();    
    _isMining = false;
}
