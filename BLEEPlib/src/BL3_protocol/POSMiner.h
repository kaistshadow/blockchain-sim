//
// Created by Hyunjin Kim on 2021/06/04.
//

#ifndef BLEEP_POSMINER_H
#define BLEEP_POSMINER_H

#include "../BL_MainEventManager.h"
#include "../utility/GlobalClock.h"
#include "../crypto/SHA256.h"
#include "POSBlock.h"
#include <ev++.h>

#include "shadow_interface.h"

namespace libBLEEP_BL {
    class POSMiner {
    public:
        POSMiner() {};
        /*********************************************************/
        /* Public API designed for high-level event requests */
        /*********************************************************/
        void AsyncMakeBlock(std::shared_ptr<POSBLOCK> templateBlock);
    };
}
#endif //BLEEP_POSMINER_H
