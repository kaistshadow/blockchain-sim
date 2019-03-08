#include "StateMachine.h"
#include "StateHandler.h"

StateMachine::StateMachine() : curState(StateEnum::uninitializedState), 
                               nextState(StateEnum::uninitializedState)
{
    
}

void StateMachine::InitStateMachine() {
    /* register proper handler(callback func) for each state) */
    stateToSignalMap[StateEnum::idleState].connect(&idleStateHandler);
    stateToSignalMap[StateEnum::exitState].connect(&exitStateHandler);

    /* initialize state-related classes */
    idleState.Init();
}

void StateMachine::StartStateMachine() {
    curState = StateEnum::idleState;
    std::cout << "Start blockchain statemachine : initial state is " << curState << "\n";
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
        if (curState == StateEnum::exitState)
            break;
        /* otherwise make a transition to nextState */
        else 
            curState = nextState;
    }
    std::cout << "Terminate blockchain statemachine " << "\n";
}
