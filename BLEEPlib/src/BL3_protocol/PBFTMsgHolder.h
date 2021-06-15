//
// Created by Hyunjin Kim on 21. 6. 15..
//

#ifndef BLEEP_PBFT_MSGHOLDER_H
#define BLEEP_PBFT_MSGHOLDER_H

#include <map>

namespace libBLEEP_BL {
    class PBFTMsgHolder {
    private:
        std::map<unsigned int, std::string> _preprepared;
    public:
        bool hasPreprepared(unsigned int n);
        void addPreprepared(unsigned int n, std::string d);
        void clearPreprepared();
    };
}

#endif //BLEEP_PBFT_MSGHOLDER_H
