#include "StateMachine.h"

#include <fcntl.h> /* Added for the nonblocking socket */
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <iostream>

using namespace simple_idle_exit_machine;

// StateMachine simple_idle_exit_machine::gStateMachine;

simple_idle_exit_machine::StateMachine::StateMachine() : curState(StateEnum::uninitialized), 
                               nextState(StateEnum::uninitialized)
{
    
}

void simple_idle_exit_machine::StateMachine::InitStateMachine() {
    /* register proper handler(callback func) for each state) */
    RegisterStateHandlers();

    /* initialize state-machine data classes */
    shadowPipeManager.Init();  // open shadow-pipe interface to receive any commands from shadow
    testPipeID = pipeManager.CreateNewPipe(); // open new pipe for local test


    // /* for debugging infinite loop */
    struct ev_loop *loop = EV_DEFAULT;
    ev_set_io_collect_interval(loop, 1);

}

void simple_idle_exit_machine::StateMachine::StartStateMachine() {
    curState = StateEnum::idle;
    std::cout << "[StateMachineLog::Start blockchain statemachine] initial state is " << curState << "\n";

    while (true) {
        
        /* Execute the handler (callback function) assigned for current state */
        std::map< StateEnum, StateSignal >::iterator it = stateToSignalMap.find(curState);
        if (it == stateToSignalMap.end()) {
            std::cout << "error: no handler is registered for the state(" << curState << ")!" << "\n";
            exit(-1);
        } else {
            StateSignal& handler = it->second;
            /* Execute handler.
               Then, retrieve a return value and put it into nextState */
            nextState = *handler(); 
        }


        /* if current state is final state(exitState), then exit the loop */
        if (curState == StateEnum::exit)
            break;
        /* otherwise make a transition to nextState */
        else {
            std::cout << "[StateMachineLog::State transition]: " << curState << " to " << nextState << "\n";
            curState = nextState;
        }
    }
    std::cout << "[StateMachineLog::Terminate blockchain statemachine] " << "\n";
}
