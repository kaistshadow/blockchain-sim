#include "PBFTSigning.h"

#include <string>

namespace libBLEEP_BL {
    std::string PBFTSecret::sign(std::string text) {return text;}
    bool PBFTPubkey::verify(std::string sig, std::string text) {return sig==text;}
}