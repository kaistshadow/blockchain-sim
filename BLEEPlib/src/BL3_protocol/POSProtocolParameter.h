//
// Created by Hyunjin Kim on 2021/06/04.
//

#ifndef BLEEP_POSPROTOCOLPARAMETER_H
#define BLEEP_POSPROTOCOLPARAMETER_H

#include "ProtocolLayer_API.h"

#include <string>

namespace libBLEEP_BL {
    class POSProtocolParameter : public ProtocolParameter {
    public:
        virtual ~POSProtocolParameter() {}
        double txGenStartAt = (double)INT_MAX;
        double txGenInterval = (double)INT_MAX;
        double slot_interval = 20;
        std::string stakeDatafile = "stakes.txt"
        unsigned int slot_epoch_cnt = 10;
        unsigned int chain_selection_block_threshold = 4;    // chain selection: 
    };
}
#endif //BLEEP_POSPROTOCOLPARAMETER_H
