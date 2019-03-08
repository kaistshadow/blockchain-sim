#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <map>
#include <boost/signals2/signal.hpp>
#include <iostream>

#include "State.h"

typedef boost::signals2::signal<StateEnum () > StateSignal;

class StateMachine {
 private:
    StateEnum curState;
    StateEnum nextState;
    std::map< StateEnum, StateSignal > stateToSignalMap;   // For each state, there exists a callback function.

 public:
    StateMachine();
    void InitStateMachine();
    void StartStateMachine();
};






#endif
