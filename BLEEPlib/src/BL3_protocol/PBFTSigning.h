//
// Created by Hyunjin Kim on 2021/06/14.
//

#ifndef BLEEP_PBFTSIGNING_H
#define BLEEP_PBFTSIGNING_H

#include <string>

namespace libBLEEP_BL {
    class PBFTSecret {
        unsigned long _id;
    public:
        void setID(unsigned long id) {_id = id;}
        std::string sign(std::string text);
    };
    class PBFTPubkey {
        unsigned long _id;
    public:
        void setID(unsigned long id) {_id = id;}
        bool verify(std::string sig, std::string text);
    };
    std::string PBFTdigest(std::string plainText);
    std::string PBFTSignature(PBFTSecret k, std::string plainText);
    bool PBFTVerify(PBFTPubkey p, std::string sig, std::string plainText);
}
#endif //BLEEP_PBFTSIGNING_H
