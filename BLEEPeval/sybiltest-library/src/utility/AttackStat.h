//
// Created by Yonggon Kim on 21/02/2021.
//

#ifndef BLEEP_ATTACKSTAT_H
#define BLEEP_ATTACKSTAT_H

#include <ev++.h>

namespace sybiltest {
    class AttackStat {
    public:
        AttackStat() {}

        int GetHijackedIncomingConnNum() { return _hijackedIncomingConnectionNum; }

        int GetHijackedOutgoingConnNum() { return _hijackedOutgoingConnectionNum; }

        void IncrementHijackedIncomingConnNum() {
            _hijackedIncomingConnectionNum += 1;
        }

        void IncrementHijackedOutgoingConnNum() {
            _hijackedOutgoingConnectionNum += 1;
        }

    private:
        int _hijackedIncomingConnectionNum = 0;
        int _hijackedOutgoingConnectionNum = 0;
        int _targetIncomingConnectionNum = -1;
        int _targetOutgoingConnectionNum = -1;
    };
}
#endif //BLEEP_ATTACKSTAT_H
