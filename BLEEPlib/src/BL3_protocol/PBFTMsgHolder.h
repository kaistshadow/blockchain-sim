//
// Created by Hyunjin Kim on 21. 6. 15..
//

#ifndef BLEEP_PBFT_MSGHOLDER_H
#define BLEEP_PBFT_MSGHOLDER_H

#include "PBFTBlock.h"

#include <map>

namespace libBLEEP_BL {
    class PBFTViewMsgHolder {

    }
    class PBFTMsgHolder {

    private:
        unsigned long _f;
    public:
        int addPreprepared(unsigned long v, unsigned int n, std::shared_ptr<PBFTBlock> m);
        int addprepared(unsigned long v, unsigned int n, std::string d, unsigned long i);
        int addCommit(unsigned long v, unsigned int n, std::string d, unsigned long i);

        // know m, preprepare for m with (v, n), 2 x f prepare(v, n, d, ...) from others
        bool predPrepared(unsigned long v, unsigned int n);
        // predPrepared(m, v, n, i), 2 x f + 1 commit(v, n, d) from others
        bool predCommittedLocal(unsigned long v, unsigned int n);

        void setFaultyLimit(unsigned long f) {
            _f = f;
        }
    };
}

#endif //BLEEP_PBFT_MSGHOLDER_H
