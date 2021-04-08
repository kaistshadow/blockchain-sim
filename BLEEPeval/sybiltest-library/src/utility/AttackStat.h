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

        int GetBenignOutgoingConnNum() { return _benignOutgoingConnectionNum; }

        void IncrementHijackedIncomingConnNum() {
            _hijackedIncomingConnectionNum += 1;
        }

        void IncrementHijackedOutgoingConnNum() {
            _hijackedOutgoingConnectionNum += 1;
        }
        void DecrementHijackedOutgoingConnNum() {
            _hijackedOutgoingConnectionNum -= 1;
        }

        void IncrementBenignOutgoingConnNum() {
            _benignOutgoingConnectionNum += 1;
        }
        void DecrementBenignOutgoingConnNum() {
            _benignOutgoingConnectionNum -= 1;
        }

    public:
        int benignNodeConnNum = 0;
        int shadowNodeConnNum = 0;
    private:
        int _hijackedIncomingConnectionNum = 0;
        int _hijackedOutgoingConnectionNum = 0;
        int _benignOutgoingConnectionNum = 0;
        int _targetIncomingConnectionNum = -1;
        int _targetOutgoingConnectionNum = -1;
    };
}
#endif //BLEEP_ATTACKSTAT_H
