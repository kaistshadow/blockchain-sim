#ifndef STATE_HANDLER_H
#define STATE_HANDLER_H

#include "State.h"

namespace singlenode_blockchain_machine {

    void RegisterStateHandlers();

    StateEnum idleStateHandler();
    StateEnum libevEventTriggeredStateHandler();
    StateEnum shadowPipeEventNotifiedStateHandler();
    StateEnum appendBlockStateHandler();
    StateEnum exitStateHandler();

};


#endif
