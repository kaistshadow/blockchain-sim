#include "StateHandler.h"
#include "../../StateMachine.h"
#include "../../../Configuration.h"
#include "../../../utility/GlobalClock.h"

#include "datamodules/Socket.h"
#include "datamodules/Message.h"

#include "utility/Assert.h"
#include <ev.h>
#include <iostream>
#include <memory>
#include <string>
#include <cassert>
#include <string.h>
#include <unistd.h>


// temp 
#include <fcntl.h> /* Added for the nonblocking socket */
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <unistd.h>

using namespace doublenode_blockchain_machine;
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

void doublenode_blockchain_machine::RegisterStateHandlers() {
    /* register proper handler(callback func) for each state) */

    /* libev handling */
    gStateMachine.stateToSignalMap[StateEnum::idle].connect(&idleStateHandler);
    gStateMachine.stateToSignalMap[StateEnum::libevEventTriggered].connect(&libevEventTriggeredStateHandler);

    /* shadow IO handling */
    gStateMachine.stateToSignalMap[StateEnum::shadowPipeEventNotified].connect(&shadowPipeEventNotifiedStateHandler);

    /* socket IO handling */
    gStateMachine.stateToSignalMap[StateEnum::receiveConnection].connect(&receiveConnectionStateHandler);
    gStateMachine.stateToSignalMap[StateEnum::socketConnected].connect(&socketConnectedStateHandler);
    gStateMachine.stateToSignalMap[StateEnum::readableSocket].connect(&readableSocketStateHandler);
    gStateMachine.stateToSignalMap[StateEnum::writableSocket].connect(&writableSocketStateHandler);
    

    /* consensus handling */
    gStateMachine.stateToSignalMap[StateEnum::appendBlock].connect(&appendBlockStateHandler);
    gStateMachine.stateToSignalMap[StateEnum::unicastConsensusMsg].connect(&unicastConsensusMsgStateHandler);

    /* termination */
    // gStateMachine.stateToSignalMap[StateEnum::exit].connect(&exitStateHandler);

}


/*****************************************  libev handling ***********************************************/
StateEnum doublenode_blockchain_machine::idleStateHandler() {
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

StateEnum doublenode_blockchain_machine::libevEventTriggeredStateHandler() {
    StateEnum nextState = StateEnum::idle;

    // check whether any valid FD event is triggered
    ShadowPipeManager& shadowPipeManager = gStateMachine.shadowPipeManager;
    PipeManager& pipeManager = gStateMachine.pipeManager;
    ListenSocketManager& listenSocketManager = gStateMachine.listenSocketManager;
    ConnectSocketManager& connectSocketManager = gStateMachine.connectSocketManager;
    DataSocketManager& dataSocketManager = gStateMachine.dataSocketManager;
    if (!shadowPipeManager.IsEventTriggered() && !pipeManager.IsEventTriggered()
        && !listenSocketManager.IsEventTriggered() && !connectSocketManager.IsEventTriggered() 
        && !dataSocketManager.IsEventTriggered()) {
        printf("No event triggered but ev_run is returned!\n");
        return StateEnum::idle;
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
                std::cout << "pipe event is triggered! However, we don't handle normal pipe event in this state machine. So ignore." << "\n";
                nextState = StateEnum::idle;
                break;
            }
        }
    }
    // check listenSocket event
    if (listenSocketManager.IsEventTriggered()) {
        switch (listenSocketManager.GetEventType()) {
        case SocketEventEnum::readEvent:
            {
                nextState = StateEnum::receiveConnection;
                break;
            }
        case SocketEventEnum::none:
        case SocketEventEnum::writeEvent:
            {
                std::cout << "invalid event is triggered. " << "\n";
                nextState = StateEnum::exit;
                break;
            }
        }
    }
    // check connectSocket event
    if (connectSocketManager.IsEventTriggered()) {
        switch (connectSocketManager.GetEventType()) {
        case SocketEventEnum::readEvent:
        case SocketEventEnum::none:
            {
                std::cout << "invalid event is triggered. " << "\n";
                nextState = StateEnum::exit;
                break;
            }
        case SocketEventEnum::writeEvent:
            {
                nextState = StateEnum::socketConnected;
                break;
            }
        }
    }
    // check dataSocket event
    if (dataSocketManager.IsEventTriggered()) {
        switch (dataSocketManager.GetEventType()) {
        case SocketEventEnum::readEvent:
            {
                nextState = StateEnum::readableSocket;
                break;
            }
        case SocketEventEnum::writeEvent:
            {
                nextState = StateEnum::writableSocket;
                break;
            }
        case SocketEventEnum::none:
            {
                std::cout << "invalid event is triggered. " << "\n";
                nextState = StateEnum::exit;
                break;
            }
        }
    }

    return nextState;
}

/*****************************************  shadow IO handling **********************************************/
StateEnum doublenode_blockchain_machine::shadowPipeEventNotifiedStateHandler() {
    StateEnum nextState = StateEnum::idle;
    ShadowPipeManager& shadowPipeManager = gStateMachine.shadowPipeManager;
    M_Assert(shadowPipeManager.IsEventTriggered(), "shadow pipe event is lost");

    int fd = shadowPipeManager.GetEventTriggeredFD();
    ShadowPipeRecvBuffer& recvBuff = shadowPipeManager.GetRecvBuff();
    shadowPipeManager.ClearEventTriggered(); // clear 

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
    std::string cmd_args_str = recvBuff.recv_str.substr(pos+1);
    std::vector<std::string> cmd_args;
    split( cmd_args_str, cmd_args, ' ');
    std::string a = cmd_args[0];
    
    // Execute received shadow command, and
    // make a state transition if necessary.
    // 'State transition' will occur as follows.
    // 1. if a shadow command called 'generateTx' is received, generate Tx and add it to TxPool.
    if (cmd_id == "generateTx") {
        boost::shared_ptr<Transaction> generatedTx(new SimpleTransaction(atoi(cmd_args[0].c_str()), atoi(cmd_args[1].c_str()), atof(cmd_args[2].c_str())));

        gStateMachine.txPool.AddTx(generatedTx);
        if (gStateMachine.txPool.GetPendingTxNum() >= block_tx_num)
            nextState = StateEnum::appendBlock;  
    } else if (cmd_id == "registerListenSocket") {
        // Initialize a listening socket
        int fd = gStateMachine.listenSocketManager.CreateListenSocket();
        std::cout << "new listening socket is established : " << fd << "\n";
    } else if (cmd_id == "registerConnectSocket") {
        // Initialize a connecting socket
        int fd = gStateMachine.connectSocketManager.CreateNonblockConnectSocket(cmd_args[0]);
        std::cout << "new connecting socket is established : " << fd << "\n";
    } else if (cmd_id == "broadcastMsg") {
        for (std::shared_ptr<DataSocket> sock : gStateMachine.dataSocketManager.GetAllDataSockets()) {
            std::shared_ptr<Message> msg = std::make_shared<Message>(cmd_args_str);
            sock->AppendMessageToSendBuff(msg);
        }
    }

    return nextState;
}

/*****************************************  socket IO handling **********************************************/
StateEnum doublenode_blockchain_machine::receiveConnectionStateHandler() {
    StateEnum nextState = StateEnum::idle;
    ListenSocketManager& listenSocketManager = gStateMachine.listenSocketManager;
    DataSocketManager& dataSocketManager = gStateMachine.dataSocketManager;
    M_Assert(listenSocketManager.IsEventTriggered(), "listen Socket event is gone?");


    int fd = listenSocketManager.GetEventTriggeredFD();
    listenSocketManager.ClearEventTriggered(); // clear 
    std::shared_ptr<ListenSocket> listenSocket = listenSocketManager.GetListenSocket(fd);
    

    // Need to execute accept on listen socket and create data socket
    // Handle pending 'accept' until there's no remaining accept requests.
    while (1) {
        int data_sfd = listenSocket->DoAccept();
        if (data_sfd == -1)
            break;
        /* create new data socket */
        dataSocketManager.CreateDataSocket(data_sfd);
    }

    return nextState;
}

StateEnum doublenode_blockchain_machine::socketConnectedStateHandler() {
    StateEnum nextState = StateEnum::idle;
    ConnectSocketManager& connectSocketManager = gStateMachine.connectSocketManager;
    DataSocketManager& dataSocketManager = gStateMachine.dataSocketManager;
    M_Assert(connectSocketManager.IsEventTriggered(), "connect Socket event is gone?");

    int fd = connectSocketManager.GetEventTriggeredFD();
    connectSocketManager.ClearEventTriggered(); // clear 

    // Convert connectSocket to dataSocket
    connectSocketManager.RemoveConnectSocket(fd); /* remove connect socket structure */
    dataSocketManager.CreateDataSocket(fd); /* create data socket structure */


    return nextState;
}

StateEnum doublenode_blockchain_machine::readableSocketStateHandler() {
    StateEnum nextState = StateEnum::idle;
    DataSocketManager& dataSocketManager = gStateMachine.dataSocketManager;
    M_Assert(dataSocketManager.IsEventTriggered(), "Data Socket event is gone?");

    int fd = dataSocketManager.GetEventTriggeredFD();
    dataSocketManager.ClearEventTriggered(); // clear 
    std::shared_ptr<DataSocket> dataSocket = dataSocketManager.GetDataSocket(fd);

    // overall process of receiving data
    // retrieve raw stream data from socket and append them into recvBuffer
    // retrieve Message from recvBuffer if possible
    // retrieve deserialized payload from generic Message, 
    // then set pointer of the state machine(argument passing between states), and transition to appropriate state handler.

    std::shared_ptr<Message> message = dataSocket->DoRecv();
    if (message) {
        switch (message->GetType()) {
        case MessageTypeEnum::String:
            std::cout << "receive string message:" << *message << "\n";
            break;
        case MessageTypeEnum::newBlock:
            std::cout << "receive newBlock" << "\n";
            break;
        default:
            break;
        }
    }

    

    return nextState;
}

StateEnum doublenode_blockchain_machine::writableSocketStateHandler() {
    StateEnum nextState = StateEnum::idle;
    DataSocketManager& dataSocketManager = gStateMachine.dataSocketManager;
    M_Assert(dataSocketManager.IsEventTriggered(), "Data Socket event is gone?");

    int fd = dataSocketManager.GetEventTriggeredFD();
    dataSocketManager.ClearEventTriggered(); // clear 
    std::shared_ptr<DataSocket> dataSocket = dataSocketManager.GetDataSocket(fd);

    // write data to dataSocket
    dataSocket->DoSend();
    

    // overall process of sending data
    // 1. serialize data as payload, to make generic Message containing the payload
    // 2. serialize Message, and push it to sendBuffer
    // 3. retrieve raw data from sendBuffer and send them into socket.

    return nextState;
}

/*****************************************  consensus handling **********************************************/
StateEnum doublenode_blockchain_machine::appendBlockStateHandler() {
    StateEnum nextState = StateEnum::idle;
    // This state is for appending a new block.
    std::cout << "appendBlock state handler executed!" << "\n";    
    M_Assert(gStateMachine.txPool.GetPendingTxNum() >= block_tx_num, "requires enough txs");
    
    std::shared_ptr<Block> newBlock(new Block("", gStateMachine.txPool.GetTxs(block_tx_num)));
    gStateMachine.ledgerManager.AppendBlock(newBlock);
    gStateMachine.txPool.RemoveTxs(newBlock->GetTransactions());

    std::cout << utility::GetGlobalClock() << ":Block is appended" << "\n";
    std::cout << *newBlock << "\n";


    // set next state as unicastConsensusMsg
    gStateMachine.newBlock = newBlock;
    nextState = StateEnum::unicastConsensusMsg;

    return nextState;
}

StateEnum doublenode_blockchain_machine::unicastConsensusMsgStateHandler() {
    StateEnum nextState = StateEnum::idle;
    // check invariants for this state
    M_Assert(gStateMachine.newBlock != nullptr, "newBlock exists");

    std::string payload = GetSerializedString(gStateMachine.newBlock);
    std::shared_ptr<Message> msg = std::make_shared<Message>(payload, MessageTypeEnum::newBlock);
    for (std::shared_ptr<DataSocket> sock : gStateMachine.dataSocketManager.GetAllDataSockets()) {
        sock->AppendMessageToSendBuff(msg);
    }
    
    return nextState;
}
