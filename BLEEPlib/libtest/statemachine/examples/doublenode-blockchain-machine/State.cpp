#include "State.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


#include "shadow_interface.h"

using namespace doublenode_blockchain_machine;

std::ostream& doublenode_blockchain_machine::operator << (std::ostream& os, const StateEnum& obj) {
    if (obj == StateEnum::uninitialized)
        os << "uninitializedState";
    /* libev handling */
    else if (obj == StateEnum::idle)
        os << "idleState";
    else if (obj == StateEnum::libevEventTriggered)
        os << "libevEventTriggeredState";

    /* shadow IO handling */
    else if (obj == StateEnum::shadowPipeEventNotified)
        os << "shadowPipeEventNotifiedState";

    /* socket IO handling */
    else if (obj == StateEnum::receiveConnection)
        os << "receiveConnectionState";
    else if (obj == StateEnum::socketConnected)
        os << "socketConnectedState";
    else if (obj == StateEnum::readableSocket)
        os << "readableSocketState";
    else if (obj == StateEnum::writableSocket)
        os << "writableSocketState";


    /* consensus handling */
    else if (obj == StateEnum::appendBlock)
        os << "appendBlockState";
    else if (obj == StateEnum::sendBlock)
        os << "sendBlockState";
    else if (obj == StateEnum::receiveBlock)
        os << "receiveBlockState";
    else if (obj == StateEnum::resolveFork)
        os << "resolveForkState";
    


    else if (obj == StateEnum::exit)
        os << "exitState";
    else
        os << "undefined(" << static_cast<std::underlying_type<StateEnum>::type>(obj) << ")";
    return os;
}

