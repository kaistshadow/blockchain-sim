#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <map>
#include <boost/signals2/signal.hpp>
#include <iostream>


#include "State.h"
#include "StateHandler.h"

#include "datamanagermodules/ShadowPipeManager.h"
#include "datamanagermodules/PipeManager.h"
#include "datamanagermodules/TxPool.h"
#include "datamanagermodules/LedgerManager.h"
#include "datamanagermodules/SocketManager.h"

#include "datamodules/Block.h"


namespace singlenode_blockchain_machine {

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

}

#endif
