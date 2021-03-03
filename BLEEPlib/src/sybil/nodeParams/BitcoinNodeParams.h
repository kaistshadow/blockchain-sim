//
// Created by ilios on 21. 2. 23..
//

#ifndef BLEEP_BITCOINNODEPARAMS_H
#define BLEEP_BITCOINNODEPARAMS_H

#include <chrono>

namespace libBLEEP_sybil {

    struct BitcoinNodeParams {
        //============================== parameters related to EREBUS attack test ======================================
        static constexpr int targetOutgoingConnNum = 5;
        static constexpr int reachableIPNum = 100000;
        static constexpr int unreachableIPNum = 1000000;
        static constexpr int shadowIPNum = 200000;
        static constexpr int addrInjectionStartTime = 15;
        static constexpr int addrInjectionDelay = 900;
        static constexpr double addrInjectionIPPerSec = 2;
        static constexpr double addrInjectionShadowRate = 0.9;
//        static constexpr std::chrono::system_clock::duration preparePhaseTimeLength = std::chrono::hours(30*24);
        static constexpr std::chrono::system_clock::duration preparePhaseTimeLength = std::chrono::hours(30*24);
    };

}

#endif //BLEEP_BITCOINNODEPARAMS_H
