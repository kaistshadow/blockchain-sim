#include "StateHandler.h"
#include "../../StateMachine.h"

#include <ev.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

using namespace simple_idle_exit_machine;

void simple_idle_exit_machine::RegisterStateHandlers() {
    /* register proper handler(callback func) for each state) */
    gStateMachine.stateToSignalMap[StateEnum::idle].connect(&idleStateHandler);
    gStateMachine.stateToSignalMap[StateEnum::exit].connect(&exitStateHandler);
    
}

StateEnum simple_idle_exit_machine::idleStateHandler() {
    std::cout << "idle state handler executed!" << "\n";

    if (!gStateMachine.shadowPipeManager.IsInitialized()) {
        std::cout << "shadow pipe manager module is not properly initialized!" << "\n";
        exit(1);
    }

    // wait for next event
    struct ev_loop *loop = EV_DEFAULT;
    ev_run (loop, EVRUN_ONCE);

    StateEnum nextState;

    // check whether any valid FD event is triggered
    FileDescriptorEventNotifier& fdEventNotifier = gStateMachine.fdEventNotifier;
    if (fdEventNotifier.IsEventTriggered()) {
        switch (fdEventNotifier.GetEventType()) {
        case FileDescriptorEventEnum::none:
            std::cout << "Error! not initialized fd event is triggered!" << "\n";
            nextState = StateEnum::exit;
            break;
        case FileDescriptorEventEnum::shadowPipeReadEvent:
        case FileDescriptorEventEnum::pipeReadEvent:
            int fd = fdEventNotifier.GetEventTriggeredFD();
            char string_read[2000];
            memset(string_read, 0, 2000);
            int n;
            n = read(fd, string_read, 2000);
            if (n == 0) {
                printf("read done : n=%d, string_read=%s\n", n, string_read);
                nextState = StateEnum::exit;
            }
            else if (n > 0 && n <= 2000) {
                printf("read : n=%d, string_read=%s\n", n, string_read);
                printf("read should be continued!\n");
                nextState = StateEnum::idle;
            }
            else {
                perror("read error");
                exit(-1);
            }
            fdEventNotifier.SetEventTriggered(false); // clear 
            break;
        }        
    } else {
        printf("Error! no event triggered but ev_run is returned!\n");
        nextState = StateEnum::exit;
    }

    return nextState;
}
StateEnum simple_idle_exit_machine::exitStateHandler() {
    std::cout << "exit state handler executed!" << "\n";
    return StateEnum::exit;
}

