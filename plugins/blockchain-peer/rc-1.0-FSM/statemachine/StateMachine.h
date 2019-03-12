#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <map>
#include <boost/signals2/signal.hpp>
#include <iostream>


#include "./examples/simple-idle-exit-machine/State.h"
#include "./examples/simple-idle-exit-machine/StateHandler.h"
using namespace simple_idle_exit_machine;

/* #include "./examples/singlenode-blockchain-machine/State.h" */
/* #include "./examples/singlenode-blockchain-machine/StateHandler.h" */
/* using namespace singlenode_blockchain_machine; */

#include "../event/FileDescriptorEventNotifier.h"
#include "../datamanagermodules/ShadowPipeManager.h"
#include "../datamanagermodules/PipeManager.h"
#include "../datamanagermodules/TxPool.h"
#include "../datamanagermodules/LedgerManager.h"



typedef boost::signals2::signal<StateEnum () > StateSignal;

class StateMachine {
 public:
    // for libev event notification
    FileDescriptorEventNotifier fdEventNotifier;

    // for shadow pipe structures
    ShadowPipeManager shadowPipeManager;

    // for normal pipe structures
    PipeManager pipeManager;

    // for pending-transaction management
    TxPool txPool;

    // for ledger management
    LedgerManager ledgerManager;

 private:
    // for logic 
    StateEnum curState;
    StateEnum nextState;

 public:
    StateMachine();
    void InitStateMachine();
    void StartStateMachine();

    std::map< StateEnum, StateSignal > stateToSignalMap;   // For each state, there exists a callback function.

};


extern StateMachine gStateMachine;



#endif
