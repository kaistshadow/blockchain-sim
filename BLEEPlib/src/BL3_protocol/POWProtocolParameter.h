// "Copyright [2021] <kaistshadow>"

//
// Created by Yonggon Kim on 2021/05/01.
//

#ifndef BLEEPLIB_SRC_BL3_PROTOCOL_POWPROTOCOLPARAMETER_H_
#define BLEEPLIB_SRC_BL3_PROTOCOL_POWPROTOCOLPARAMETER_H_

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
#endif  // BLEEPLIB_SRC_BL3_PROTOCOL_POWPROTOCOLPARAMETER_H_
