// "Copyright [2021] <kaistshadow>"

//
// Created by ilios on 21. 2. 23..
//

#ifndef BLEEPEVAL_SYBILTEST_APP_MONERO_MONERONODEPARAMS_H_
#define BLEEPEVAL_SYBILTEST_APP_MONERO_MONERONODEPARAMS_H_

#include <chrono>

namespace sybiltest {

struct MoneroNodeParams {
    //============================== parameters related to EREBUS attack test ======================================
    static constexpr int targetPort = 28080;
    static constexpr int targetOutgoingConnNum = 10;
    static constexpr int reachableIPNum = 1000;
    static constexpr int unreachableIPNum = 1000;
    static constexpr int shadowIPNum = 2000;
    static constexpr int addrInjectionStartTime = 15;
    static constexpr int addrInjectionDelay = 600;
    static constexpr double addrInjectionIPPerSec = 2;
    static constexpr double addrInjectionShadowRate = 0.9;
    // static constexpr std::chrono::system_clock::duration preparePhaseTimeLength = std::chrono::hours(30*24);
    static constexpr std::chrono::system_clock::duration preparePhaseTimeLength = std::chrono::hours(1);
};
} // namespace sybiltest

#endif  // BLEEPEVAL_SYBILTEST_APP_MONERO_MONERONODEPARAMS_H_
