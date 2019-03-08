#include "State.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


#include "../../shadow_interface/shadow_interface.h"

IdleState idleState;

std::ostream& operator << (std::ostream& os, const StateEnum& obj) {
    if (obj == StateEnum::uninitializedState)
        os << "UninitializedState";
    else if (obj == StateEnum::idleState)
        os << "idleState";
    else if (obj == StateEnum::exitState)
        os << "exitState";
    else
        os << static_cast<std::underlying_type<StateEnum>::type>(obj);
    return os;
}


void IdleState::_PipeIOCallback(ev::io &w, int revents) {
    _eventType = LibevEventEnum::pipeReadEvent;
    _eventTriggeredFD = w.fd;
    std::cout << "pipeiocallback called!" << "\n";

    // struct ev_loop *loop = EV_DEFAULT;
    // ev_break(loop, EVBREAK_ALL);

    // int fd = GetEventTriggeredFD();
    // char string_read[2000];
    // memset(string_read, 0, 2000);
    // int n;
    // n = read(fd, string_read, 2000);
    // if (n == 0) {
    //     printf("read done : n=%d, string_read=%s\n", n, string_read);
    // }
    // else if (n > 0 && n <= 2000) {
    //     printf("read : n=%d, string_read=%s\n", n, string_read);
    //     printf("read should be continued!\n");
    // }
    // else {
    //     perror("read error");
    //     exit(-1);
    // }
}

void IdleState::Init() {
    int result = pipe2(_pipeFD, 0);
    if (result < 0) {
        perror("pipe ");
        exit(1);
    }

    fcntl(_pipeFD[0], F_SETFL, O_NONBLOCK); /* Change the fd into non-blocking state. */ 

    _pipeIOWList.emplace_back(); 
    ev::io& pipeIOW = _pipeIOWList.back();
    pipeIOW.set<IdleState, &IdleState::_PipeIOCallback> (this);
    pipeIOW.start(_pipeFD[0], ev::READ);

    _initialized = true;
}
