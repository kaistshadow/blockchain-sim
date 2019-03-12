#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <type_traits>
#include <ev++.h>
#include <list>

namespace singlenode_blockchain_machine {

    enum class StateEnum {
        uninitialized,
            idle,
            shadowPipeEventNotified,
            appendBlock,
            exit
            };

    std::ostream& operator << (std::ostream& os, const StateEnum& obj);

}


#endif

