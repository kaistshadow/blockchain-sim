//
// Created by Yonggon Kim on 2021/05/01.
//

#ifndef BLEEP_POWPROTOCOLPARAMETER_H
#define BLEEP_POWPROTOCOLPARAMETER_H

#include "ProtocolLayer_API.h"

namespace libBLEEP_BL {
    class POWProtocolParameter : public ProtocolParameter {
    public:
        virtual ~POWProtocolParameter() {}
        double txGenStartAt = (double)INT_MAX;
        double txGenInterval = (double)INT_MAX;
        double miningtime = 2;
        int miningnodecnt = 1;
    };
}
#endif //BLEEP_POWPROTOCOLPARAMETER_H
