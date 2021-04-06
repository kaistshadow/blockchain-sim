//
// Created by ilios on 21. 2. 23..
//

#ifndef BLEEP_MONERONODEPARAMS_H
#define BLEEP_MONERONODEPARAMS_H

#include <chrono>

namespace sybiltest {

    struct MoneroNodeParams {
        //============================== parameters related to EREBUS attack test ======================================
        static constexpr int targetPort = 28080;
        static constexpr int targetOutgoingConnNum = 2;
        static constexpr int reachableIPNum = 1000;
        static constexpr int unreachableIPNum = 10000;
        static constexpr int shadowIPNum = 2000;
        static constexpr int addrInjectionStartTime = 15;
        static constexpr int addrInjectionDelay = 600;
        static constexpr double addrInjectionIPPerSec = 2;
        static constexpr double addrInjectionShadowRate = 0.9;
//        static constexpr std::chrono::system_clock::duration preparePhaseTimeLength = std::chrono::hours(30*24);
        static constexpr std::chrono::system_clock::duration preparePhaseTimeLength = std::chrono::minutes(5);
    };

}

#endif //BLEEP_MONERONODEPARAMS_H
