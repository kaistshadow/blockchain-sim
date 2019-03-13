#include "../../../Configuration.h"
#include "StateHandler.h"
#include "../../StateMachine.h"
#include "../../../utility/GlobalClock.h"

#include "utility/Assert.h"
#include <ev.h>
#include <iostream>
#include <memory>
#include <string>
#include <cassert>
#include <string.h>
#include <unistd.h>

using namespace singlenode_blockchain_machine;
using namespace libBLEEP;

static size_t split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    size_t pos = txt.find( ch );
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while( pos != std::string::npos ) {
        strs.push_back( txt.substr( initialPos, pos - initialPos ) );
        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
    }

    // Add the last one
    strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );

    return strs.size();
}

void singlenode_blockchain_machine::RegisterStateHandlers() {
    /* register proper handler(callback func) for each state) */
    gStateMachine.stateToSignalMap[StateEnum::idle].connect(&idleStateHandler);
    gStateMachine.stateToSignalMap[StateEnum::libevEventTriggered].connect(&libevEventTriggeredStateHandler);
    gStateMachine.stateToSignalMap[StateEnum::shadowPipeEventNotified].connect(&shadowPipeEventNotifiedStateHandler);
    gStateMachine.stateToSignalMap[StateEnum::appendBlock].connect(&appendBlockStateHandler);
    gStateMachine.stateToSignalMap[StateEnum::exit].connect(&exitStateHandler);
    
}

StateEnum singlenode_blockchain_machine::idleStateHandler() {
    std::cout << "idle state handler executed!" << "\n";

    if (!gStateMachine.shadowPipeManager.IsInitialized()) {
        std::cout << "shadow pipe manager module is not properly initialized!" << "\n";
        exit(1);
    }

    // wait for next event
    struct ev_loop *loop = EV_DEFAULT;

    // blocking by epoll_wait. returned after handling io-callback function.
    ev_run (loop, EVRUN_ONCE);

    StateEnum nextState = StateEnum::libevEventTriggered;
    return nextState;
}

StateEnum singlenode_blockchain_machine::libevEventTriggeredStateHandler() {
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
                nextState = StateEnum::shadowPipeEventNotified;
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
        case PipeEventEnum::none:
        case PipeEventEnum::writeEvent:
            {
                std::cout << "pipe event is triggered! we don't handle this." << "\n";
                nextState = StateEnum::idle;
                break;
            }
        }
    }

    return nextState;
}


StateEnum singlenode_blockchain_machine::shadowPipeEventNotifiedStateHandler() {
    StateEnum nextState = StateEnum::idle;
    ShadowPipeManager& shadowPipeManager = gStateMachine.shadowPipeManager;
    M_Assert(shadowPipeManager.IsEventTriggered(), "shadow pipe event is lost");


    int fd = shadowPipeManager.GetEventTriggeredFD();
    ShadowPipeRecvBuffer& recvBuff = shadowPipeManager.GetRecvBuff();
    shadowPipeManager.SetEventTriggered(false); // clear 

    char string_read[2000];  
    memset(string_read, 0, 2000);
    int n;

    // receive command length
    int length = 0;
    n = read(fd,&length,sizeof(int));
    if (n == -1 && errno != EAGAIN){
        perror("read - non blocking \n");
        std::cout << "errno=" << errno << "\n";
        exit(-1);
    }
    else if (n == 0) {
        std::cout << "error: pipe disconnected" << "\n";
        exit(-1);
    }
    else if (n > 0) {
        recvBuff.message_len = length;
        recvBuff.received_len = 0;
        recvBuff.recv_str = "";
    }

    // receive command
    int total_recv_size = recvBuff.received_len;
    int numbytes = 0;
    while(1) {
        int recv_size = std::min(2000, recvBuff.message_len - total_recv_size);
        numbytes = read(fd, string_read, recv_size);
        if (numbytes > 0) {
            total_recv_size += numbytes;
            recvBuff.recv_str.append(string_read, numbytes);
        }
        else if (numbytes == 0) {
            perror("recv_cmd fail (pipe closed) \n");
            exit(-1);
        }
        else if (numbytes < 0) {
            perror("recv_cmd fail \n");
            std::cout << "errno=" << errno << "\n";
            exit(-1);
        }

        if (total_recv_size == recvBuff.message_len)
            break;
    }

    if (recvBuff.message_len != total_recv_size) {
        recvBuff.received_len = total_recv_size;
        std::cout << "error: received only part of command" << "received_len:" << recvBuff.received_len << ", message_len:" << recvBuff.message_len << "\n";
        exit(-1);
    }

    // parse command 
    std::string delimiter = ":";
            
    size_t pos = 0;
    std::string token;
    if ( (pos = recvBuff.recv_str.find(delimiter)) == std::string::npos ) {
        std::cout << "error: wrong format for shadow-generated command (" << recvBuff.recv_str << ")" << "\n";
        exit(-1);
    }
    else {
        std::cout << "Successfully received shadow-generated command (" << recvBuff.recv_str << ")" << "\n";
    }
            
    std::string cmd_id = recvBuff.recv_str.substr(0, pos);
    std::string cmd_args_str = recvBuff.recv_str.substr(pos);
    std::vector<std::string> cmd_args;
    split( cmd_args_str, cmd_args, ' ');
    
    // New state transition.
    // 'State transition' will occur as follows.
    // 1. if a shadow command called 'generateTx' is received, move to 'generateTx' state.
    if (cmd_id == "generateTx") {
        std::shared_ptr<Transaction> generatedTx(new SimpleTransaction(atoi(cmd_args[0].c_str()), atoi(cmd_args[1].c_str()), atof(cmd_args[2].c_str())));

        gStateMachine.txPool.AddTx(generatedTx);
        if (gStateMachine.txPool.GetPendingTxNum() >= block_tx_num)
            nextState = StateEnum::appendBlock;  
    }

    return nextState;
}

StateEnum singlenode_blockchain_machine::appendBlockStateHandler() {
    std::cout << "appendBlock state handler executed!" << "\n";    
    // This state is for appending a new block.
    M_Assert(gStateMachine.txPool.GetPendingTxNum() >= block_tx_num, "requires enough txs");
    
    std::shared_ptr<Block> newBlock(new Block("", gStateMachine.txPool.GetTxs(block_tx_num)));
    gStateMachine.ledgerManager.AppendBlock(newBlock);
    gStateMachine.txPool.RemoveTxs(newBlock->GetTransactions());

    std::cout << utility::GetGlobalClock() << ":Block is appended" << "\n";
    std::cout << *newBlock << "\n";

    return StateEnum::idle;
}

StateEnum singlenode_blockchain_machine::exitStateHandler() {
    std::cout << "exit state handler executed!" << "\n";
    return StateEnum::exit;
}

