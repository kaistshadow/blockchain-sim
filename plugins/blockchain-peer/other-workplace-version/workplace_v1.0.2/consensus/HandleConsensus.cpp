#include "HandleConsensus.h"
#include "HandleConsensus_POW.h"

std::shared_ptr<HandleConsensus>  HandleConsensus::create(const int type) {
    if (type == HANDLE_CONSENSUS_POW) {
        return std::shared_ptr<HandleConsensus>(new HandleConsensus_POW());
    }
    else {
        exit(-1);
    }
}
