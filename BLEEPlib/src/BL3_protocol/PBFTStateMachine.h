//
// Created by Hyunjin Kim on 2021/06/09.
//

#ifndef BLEEP_PBFT_STATE_MACHINE_H
#define BLEEP_PBFT_STATE_MACHINE_H

namespace libBLEEP_BL {
    class PBFTStateMachine {
    private:
        unsigned int sequence;
        unsigned int view;

        unsigned int _nodeCount;

        State s;

    public:
        PBFTStateMachine(unsigned int nodeCount) : _nodeCount(nodeCount) {
            
        }
        State StateChange(Input i);
    };
}

#endif //BLEEP_PBFT_STATE_MACHINE_H
