#ifndef STATE_HANDLER_H
#define STATE_HANDLER_H

#include "State.h"

namespace simple_idle_exit_machine {

    void RegisterStateHandlers();

    StateEnum idleStateHandler();
    StateEnum exitStateHandler();

}





#endif
