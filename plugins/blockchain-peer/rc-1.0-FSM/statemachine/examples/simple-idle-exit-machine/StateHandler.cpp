#include "StateHandler.h"
#include "../../StateMachine.h"

#include <ev.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

using namespace simple_idle_exit_machine;
using namespace libBLEEP;

void simple_idle_exit_machine::RegisterStateHandlers() {
    /* register proper handler(callback func) for each state) */
    gStateMachine.stateToSignalMap[StateEnum::idle].connect(&idleStateHandler);
    gStateMachine.stateToSignalMap[StateEnum::libevEventTriggered].connect(&libevEventTriggeredStateHandler);
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

    StateEnum nextState = StateEnum::libevEventTriggered;

    return nextState;

}

StateEnum simple_idle_exit_machine::libevEventTriggeredStateHandler() {
    StateEnum nextState = StateEnum::idle;

    // check whether any valid FD event is triggered
    ShadowPipeManager& shadowPipeManager = gStateMachine.shadowPipeManager;
    PipeManager& pipeManager = gStateMachine.pipeManager;
    if (!shadowPipeManager.IsEventTriggered() && !pipeManager.IsEventTriggered()) {
        printf("No event triggered but ev_run is returned!\n");
        return nextState;
    }

    // check shadow pipe event
    if (shadowPipeManager.IsEventTriggered()) {
        switch (shadowPipeManager.GetEventType()) {
        case ShadowPipeEventEnum::readEvent:
            {
                int fd = shadowPipeManager.GetEventTriggeredFD();
                char string_read[2000];
                memset(string_read, 0, 2000);
                int n;
                n = read(fd, string_read, 2000);
                if (n == 0) {
                    printf("pipe closed : n=%d, string_read=%s\n", n, string_read);
                    nextState = StateEnum::exit;
                }
                else if (n > 0 && n <= 2000) {
                    printf("read : n=%d, string_read=%s\n", n, string_read);
                    nextState = StateEnum::idle;
                }
                else {
                    perror("read error");
                    exit(-1);
                }
                shadowPipeManager.SetEventTriggered(false); // clear 
                break;
            }
        case ShadowPipeEventEnum::none:
        case ShadowPipeEventEnum::writeEvent: 
            {
                std::cout << "Error! no valid shadow pipe event is triggered!" << "\n";
                nextState = StateEnum::exit;
                break;
            }
        }
    }

    // check pipe event
    if (pipeManager.IsEventTriggered()) {
        switch (pipeManager.GetEventType()) {
        case PipeEventEnum::readEvent:
            {
                int fd = pipeManager.GetEventTriggeredFD();
                char string_read[2000];
                memset(string_read, 0, 2000);
                int n;
                n = read(fd, string_read, 2000);
                if (n == 0) {
                    printf("pipe closed : n=%d, string_read=%s\n", n, string_read);
                    nextState = StateEnum::exit;
                }
                else if (n > 0 && n <= 2000) {
                    printf("read : n=%d, string_read=%s\n", n, string_read);
                    nextState = StateEnum::idle;
                }
                else {
                    perror("read error");
                    exit(-1);
                }
                pipeManager.SetEventTriggered(false); // clear 
                break;
            }
        case PipeEventEnum::none:
        case PipeEventEnum::writeEvent:
            {
                std::cout << "Error! no valid pipe event is triggered!" << "\n";
                nextState = StateEnum::exit;
                break;
            }
        }
    }

    return nextState;
}

StateEnum simple_idle_exit_machine::exitStateHandler() {
    std::cout << "exit state handler executed!" << "\n";
    return StateEnum::exit;
}

