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

        std::string configFile = "config.txt";
        unsigned long consensusNodeID = 0;
        double phase_interval = 20;
    };
}
#endif //BLEEP_PBFTPROTOCOLPARAMETER_H
