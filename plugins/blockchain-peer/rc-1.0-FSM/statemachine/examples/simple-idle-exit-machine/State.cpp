#include "State.h"
#include <string.h>


using namespace simple_idle_exit_machine;


std::ostream& simple_idle_exit_machine::operator << (std::ostream& os, const StateEnum& obj) {
    if (obj == StateEnum::uninitialized)
        os << "UninitializedState";
    else if (obj == StateEnum::idle)
        os << "idleState";
    else if (obj == StateEnum::exit)
        os << "exitState";
    else
        os << static_cast<std::underlying_type<StateEnum>::type>(obj);
    return os;
}


