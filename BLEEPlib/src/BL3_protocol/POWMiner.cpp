// "Copyright [2021] <kaistshadow>"

//
// Created by Yonggon Kim on 2021/04/25.
//

#include "../utility/Assert.h"
#include "../utility/Random.h"
#include "shadow_interface.h"

#include "POWMiner.h"

using namespace libBLEEP_BL;

void POWMiner::AsyncEmulateBlockMining(std::shared_ptr<POWBlock> candidateBlk, double lambda) {
    if (lambda < 0) {
        libBLEEP::M_Assert(0, "average value of the time should be positive value");
    }

    double waiting_time = libBLEEP::get_global_random_source().get_exp_value(lambda);

    _candidateBlk = candidateBlk;
    _timer.set<POWMiner, &POWMiner::_timerCallback>(this);
    _timer.set(waiting_time, 0.);
    _timer.start();
    std::cout << "block mining timer started!" << "\n";
    _isMining = true;

    //  append shadow api log
    char buf[256];
    sprintf(buf, "API,AsyncEmulateBlockMining,%f", lambda);
    shadow_push_eventlog(buf);
}

void POWMiner::StopMining() {
    _timer.stop();
    _isMining = false;

    //  append shadow api log
    char buf[256];
    sprintf(buf, "API,StopMining");
    shadow_push_eventlog(buf);
}

