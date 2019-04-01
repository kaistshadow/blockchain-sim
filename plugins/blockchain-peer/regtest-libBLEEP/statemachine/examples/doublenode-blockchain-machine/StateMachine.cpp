#include "StateMachine.h"
#include "../../../utility/GlobalClock.h"

#include <fcntl.h> /* Added for the nonblocking socket */
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <iostream>

using namespace doublenode_blockchain_machine;

std::string doublenode_blockchain_machine::GetSerializedString(std::shared_ptr<MyBlock> blk) {
    std::string serial_str;
    MyBlock* block_ptr = blk.get();
    // serialize obj into an std::string
    boost::iostreams::back_insert_device<std::string> inserter(serial_str);
    boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
    boost::archive::binary_oarchive oa(s);
    oa << block_ptr;
    s.flush();
    return serial_str;
}

std::shared_ptr<MyBlock> doublenode_blockchain_machine::GetDeserializedMyBlock(std::string str) {
    MyBlock* block;
    // wrap buffer inside a stream and deserialize string_read into obj
    boost::iostreams::basic_array_source<char> device(str.c_str(), str.size());
    boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
    boost::archive::binary_iarchive ia(s);
    ia >> block;

    return std::shared_ptr<MyBlock>(block);
}


// StateMachine doublenode_blockchain_machine::gStateMachine;

StateMachine::StateMachine() : curState(StateEnum::uninitialized), 
                               nextState(StateEnum::uninitialized)
{
    
}

void StateMachine::InitStateMachine() {
    /* register proper handler(callback func) for each state) */
    RegisterStateHandlers();

    /* initialize state-machine data classes */
    shadowPipeManager.Init();  // open shadow-pipe interface to receive any commands from shadow
    testPipeID = pipeManager.CreateNewPipe(); // open new pipe for local test


    // /* for debugging infinite loop */
    // struct ev_loop *loop = EV_DEFAULT;
    // ev_set_io_collect_interval(loop, 1);

}

void StateMachine::StartStateMachine() {
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
            std::cout << utility::GetGlobalClock() << ":[StateMachineLog::State transition]: " << curState << " to " << nextState << "\n";
            curState = nextState;
        }
    }
    std::cout << "[StateMachineLog::Terminate blockchain statemachine] " << "\n";
}
