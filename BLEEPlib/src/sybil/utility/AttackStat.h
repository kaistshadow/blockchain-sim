//
// Created by Yonggon Kim on 21/02/2021.
//

#ifndef BLEEP_ATTACKPOLICY_H
#define BLEEP_ATTACKPOLICY_H

namespace libBLEEP_sybil {
    class AttackStat {
    public:
        AttackStat() {}

        void SetTargetIncomingConnNum(int num) { _targetIncomingConnectionNum = num; }
        void SetTargetOutgoingConnNum(int num) { _targetOutgoingConnectionNum = num; }
    protected:
        void IncrementSuccessNum() { _targetNum += 1; }

    private:
        int _targetIncomingConnectionNum = -1;
        int _targetOutgoingConnectionNum = -1;
    };
}
#endif //BLEEP_ATTACKPOLICY_H
