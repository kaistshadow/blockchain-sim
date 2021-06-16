//
// Created by Hyunjin Kim on 21. 6. 15..
//

#ifndef BLEEP_PBFT_MSGHOLDER_H
#define BLEEP_PBFT_MSGHOLDER_H

#include "PBFTBlock.h"

#include <map>

namespace libBLEEP_BL {
    class PBFTMsgHolder {
    private:
        std::map<unsigned int, std::string> _preprepared;
    public:
        bool hasPreprepared(unsigned int n);
        void addPreprepared(unsigned int n, std::string d);
        void clearPreprepared();

        bool predPrepared(std::shared_ptr<PBFTBlock> m, unsigned long v, unsigned int n, unsigned long i);
        bool predCommitted(std::shared_ptr<PBFTBlock> m, unsigned long v, unsigned int n);
        bool predCommittedLocal(std::shared_ptr<PBFTBlock> m, unsigned long v, unsigned int n , unsigned long i);
    };
}

#endif //BLEEP_PBFT_MSGHOLDER_H
