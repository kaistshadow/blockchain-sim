#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <type_traits>
#include <ev++.h>
#include <list>

namespace simple_idle_exit_machine {

    enum class StateEnum {
        uninitialized,
            idle,
            libevEventTriggered,
            exit
            };

    std::ostream& operator << (std::ostream& os, const StateEnum& obj);

        /* pipe fds */
        /* int _pipeFD[2]; */

}
#endif

