#include "State.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


#include "shadow_interface.h"

using namespace singlenode_blockchain_machine;

std::ostream& singlenode_blockchain_machine::operator << (std::ostream& os, const StateEnum& obj) {
    if (obj == StateEnum::uninitialized)
        os << "uninitializedState";
    else if (obj == StateEnum::idle)
        os << "idleState";
    else if (obj == StateEnum::libevEventTriggered)
        os << "libevEventTriggeredState";
    else if (obj == StateEnum::shadowPipeEventNotified)
        os << "shadowPipeEventNotifiedState";
    else if (obj == StateEnum::appendBlock)
        os << "appendBlockState";
    else if (obj == StateEnum::exit)
        os << "exitState";
    else
        os << "undefined(" << static_cast<std::underlying_type<StateEnum>::type>(obj) << ")";
    return os;
}

