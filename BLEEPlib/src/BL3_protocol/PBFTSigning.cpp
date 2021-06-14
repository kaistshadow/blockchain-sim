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
}