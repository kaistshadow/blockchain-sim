#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <type_traits>
#include <ev++.h>
#include <list>

namespace doublenode_blockchain_machine {

    enum class StateEnum {
        uninitialized,
            /* libev handling */
            idle,
            libevEventTriggered,

            /* shadow IO handling */
            shadowPipeEventNotified,

            /* socket IO handling */
            receiveConnection, /* connection */
            socketConnected,   /* connection */
            readableSocket,    /* recv */
            writableSocket,    /* send */

            /* consensus handling */
            appendBlock,
            broadcastConsensusMsg,
            receiveConsensusMsg,
            resolveFork,

            exit
            };

    std::ostream& operator << (std::ostream& os, const StateEnum& obj);

}


#endif

