#include "PBFTSigning.h"

#include <sstream>
#include <string>

namespace libBLEEP_BL {
    std::string PBFTSecret::sign(std::string text) {
        std::ostringstream oss;
        oss << _id << text;
        return oss.str();
    }
    bool PBFTPubkey::verify(std::string sig, std::string text) {
        std::ostringstream oss;
        oss << _id << text;
        return sig==oss.str();
    }

    std::string PBFTDigest(std::string plainText) {
        return plainText;
    }
    std::string PBFTSignature(PBFTSecret k, std::string plainText) {
        std::string d = PBFTDigest(plainText);
        return k.sign(d);
    }
    bool PBFTVerify(PBFTPubkey p, std::string sig, std::string plainText) {
        std::string d = PBFTDigest(plainText);
        return p.verify(sig, d);
    }
}