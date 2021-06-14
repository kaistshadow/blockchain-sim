//
// Created by Hyunjin Kim on 2021/06/14.
//

#ifndef BLEEP_PBFTSIGNING_H
#define BLEEP_PBFTSIGNING_H

#include <string>

namespace libBLEEP_BL {
    class PBFTSecret {
    public:
        std::string sign(std::string text);
    };
    class PBFTPubkey {
    public:
        bool verify(std::string sig, std::string text);
    };
}
#endif //BLEEP_PBFTSIGNING_H
