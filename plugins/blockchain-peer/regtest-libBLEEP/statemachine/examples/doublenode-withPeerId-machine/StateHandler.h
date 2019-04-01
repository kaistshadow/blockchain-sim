#ifndef STATE_HANDLER_H
#define STATE_HANDLER_H

#include "State.h"

namespace doublenode_blockchain_machine {

    void RegisterStateHandlers();

    /* libev handling */
    StateEnum idleStateHandler();
    StateEnum libevEventTriggeredStateHandler();

    /* shadow IO handling */
    StateEnum shadowPipeEventNotifiedStateHandler();

    /* socket IO handling */
    StateEnum receiveConnectionStateHandler();
    StateEnum socketConnectedStateHandler();
    StateEnum readableSocketStateHandler();
    StateEnum writableSocketStateHandler();

    /* consensus handling */
    StateEnum appendBlockStateHandler();
    StateEnum broadcastConsensusMsgStateHandler();

    /* termination */
    StateEnum exitStateHandler();

};


#endif
