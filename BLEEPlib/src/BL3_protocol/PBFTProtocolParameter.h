//
// Created by Hyunjin Kim on 2021/06/08.
//

#ifndef BLEEP_PBFTPROTOCOLPARAMETER_H
#define BLEEP_PBFTPROTOCOLPARAMETER_H

#include "ProtocolLayer_API.h"

#include <string>

namespace libBLEEP_BL {
    class PBFTProtocolParameter : public ProtocolParameter {
    public:
        virtual ~PBFTProtocolParameter() {}
        double txGenStartAt = (double)INT_MAX;
        double txGenInterval = (double)INT_MAX;

        unsigned long consensusId = 0;
        std::string configFile = "config.txt";
        double phase_interval = 20;
    };
}
#endif //BLEEP_PBFTPROTOCOLPARAMETER_H
