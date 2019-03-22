#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <map>
#include <boost/signals2/signal.hpp>
#include <iostream>


/* #include "./examples/simple-idle-exit-machine/State.h" */
/* #include "./examples/simple-idle-exit-machine/StateHandler.h" */
/* using namespace simple_idle_exit_machine; */

#include "./examples/singlenode-blockchain-machine/State.h"
#include "./examples/singlenode-blockchain-machine/StateHandler.h"
using namespace singlenode_blockchain_machine;

/* #include "./examples/doublenode-blockchain-machine/State.h" */
/* #include "./examples/doublenode-blockchain-machine/StateHandler.h" */
/* using namespace doublenode_blockchain_machine; */

#include "datamanagermodules/ShadowPipeManager.h"
#include "datamanagermodules/PipeManager.h"
#include "datamanagermodules/TxPool.h"
#include "datamanagermodules/LedgerManager.h"
#include "datamanagermodules/SocketManager.h"

#include "datamodules/Block.h"


typedef boost::signals2::signal<StateEnum () > StateSignal;

class StateMachine {
 public:
    // for shadow pipe structures
    libBLEEP::ShadowPipeManager shadowPipeManager;

    // for normal pipe structures
    libBLEEP::PipeManager pipeManager;
    // for pipe test
    libBLEEP::PipeID testPipeID;

    // for pending-transaction management
    libBLEEP::TxPool txPool;

    // for ledger management
    libBLEEP::LedgerManager ledgerManager;

    // for listening socket management
    libBLEEP::ListenSocketManager listenSocketManager;

    // for connecting socket management
    libBLEEP::ConnectSocketManager connectSocketManager;

    // for data socket management
    libBLEEP::DataSocketManager dataSocketManager;

 public:
    // pointer for intermediate values (arguments passing between states)
    std::shared_ptr<libBLEEP::Block> newBlock = nullptr;
    // std::shared_ptr<Block> receivedBlk;
    // std::shared_ptr<ConsensusMsg> receivedMsg;

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
