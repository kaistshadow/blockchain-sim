//
// Created by Hyunjin Kim on 21. 6. 15..
//

#ifndef BLEEP_PBFT_MSGHOLDER_H
#define BLEEP_PBFT_MSGHOLDER_H

#include "PBFTBlock.h"

#include <map>
#include <memory>
#include <string>
#include <set>

namespace libBLEEP_BL {
    class PBFTViewMsgHolder {
    private:
        std::map<unsigned int, std::shared_ptr<PBFTBlock>> _preprepared;
        std::map<unsigned int, std::shared_ptr<std::map<std::string, std::shared_ptr<std::set<unsigned long>>>>> _prepared;
        std::map<unsigned int, std::shared_ptr<std::map<std::string, std::shared_ptr<std::set<unsigned long>>>>> _committed;

        unsigned long _f;
    public:
        PBFTViewMsgHolder(unsigned long f) { _f = f; }

        int addPreprepared(unsigned int n, std::shared_ptr<PBFTBlock> m);
        int addPrepared(unsigned int n, std::string d, unsigned long i);
        int addCommit(unsigned int n, std::string d, unsigned long i);

        bool hasPreprepared(unsigned int n);
        bool hasCommit(unsigned int n, std::string d, unsigned long i);
        // know m, preprepare for m with (v, n), 2 x f prepare(v, n, d, ...) from others
        bool predPrepared(unsigned int n);
        // predPrepared(m, v, n, i), 2 x f + 1 commit(v, n, d) from others
        bool predCommittedLocal(unsigned int n);
        std::shared_ptr<PBFTBlock> getMessage(unsigned int n);
    };
    class PBFTMsgHolder {

    private:
        std::map<unsigned long, std::shared_ptr<PBFTViewMsgHolder>> viewMaps;
        std::map<std::pair<unsigned long, unsigned int>, bool> predPreparedOnceFlag;
        std::map<std::pair<unsigned long, unsigned int>, bool> predCommittedLocalOnceFlag;

        unsigned long _f;
    public:
        PBFTMsgHolder() { _f = 0; }
        int addPreprepared(unsigned long v, unsigned int n, std::shared_ptr<PBFTBlock> m);
        int addPrepared(unsigned long v, unsigned int n, std::string d, unsigned long i);
        int addCommit(unsigned long v, unsigned int n, std::string d, unsigned long i);

        bool hasPreprepared(unsigned long v, unsigned int n);
        bool hasCommit(unsigned long v, unsigned int n, std::string d, unsigned long i);
        // know m, preprepare for m with (v, n), 2 x f prepare(v, n, d, ...) from others
        bool predPrepared(unsigned long v, unsigned int n);
        bool predPreparedOnce(unsigned long v, unsigned int n);
        // predPrepared(m, v, n, i), 2 x f + 1 commit(v, n, d) from others
        bool predCommittedLocal(unsigned long v, unsigned int n);
        bool predCommittedLocalOnce(unsigned long v, unsigned int n);

        void setFaultyLimit(unsigned long f) {
            _f = f;
        }

        std::shared_ptr<PBFTBlock> getMessage(unsigned long v, unsigned int n);
    };
}

#endif //BLEEP_PBFT_MSGHOLDER_H