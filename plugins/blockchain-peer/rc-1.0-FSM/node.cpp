#include <iostream>
#include <memory>
#include "Configuration.h"
#include "utility/NodeInfo.h"
#include "utility/ArgsManager.h"
#include "utility/GlobalClock.h"
#include "statemachine/StateMachine.h"

// =================  Blockchain Node Configuration Start ===================

// general configuration
bool amIFullNode = true;
bool amINetworkParticipantNode = false;
int generateTxNum = 0;
int injectedTxNum = 0;
int generateTxTime = 0;

// configurations for proxy-based network
bool amIProxyNode = false;

// configurations for gossip-based network
bool amIContactNode = false;
bool isGossipShutdown = false;
int shutdownTime = -1;

// Blockchain Consensus Configuration
int block_tx_num = 5;
int mining_time = 10;
std::string mining_time_dev = "2.0";

// Blockchain Transaction Configuration
TransactionType txType = EnumSimpleTransaction;

//This function continously writes Capital Alphabet into fd[1]
//Waits if no more space is available

// void *writer_ABC(void*)
// {
//     int     result;
//     char    ch='A';

//     while(1){
//         result = write (gStateMachine.pipeManager.GetPipeWriteFD(), &ch,1);
//         if (result != 1){
//             perror ("write");
//             exit (2);
//         }

//         printf ("Writer_ABC: %c\n", ch);
//         // break;
//         if(ch == 'Z')
//             break;
//             // ch = 'A'-1;

//         ch++;
//     }
//     // sleep(1);
//     close(gStateMachine.pipeManager.GetPipeWriteFD());
//     printf("socket closed!\n");
//     return 0;
// }

int main(int argc, char *argv[]) {
    gArgs.ParseParameters(argc, argv);

    if (gArgs.IsArgSet("-?") || gArgs.IsArgSet("-h") ||  gArgs.IsArgSet("-help") || gArgs.IsArgSet("-version")) {
        std::string strUsage = gArgs.HelpMessage();
        std::cout << strUsage << "\n";
        return 0;
    }

    if (gArgs.IsArgSet("-txtype")) {
        std::string transactiontype = gArgs.GetArg("-txtype","");
        if (transactiontype == "simple") {
            txType = EnumSimpleTransaction;
        }
        else if (transactiontype == "useless") {
            txType = EnumUselessTransaction;
        }
        else // default is 'SimpleTransaction'
            txType = EnumSimpleTransaction;
    }

    if (gArgs.GetBoolArg("-networkparticipant", false)) {
        amIFullNode = false;
        amINetworkParticipantNode = true;
    } else {
        amIFullNode = true;
        amINetworkParticipantNode = false;
    }

    amIProxyNode = gArgs.GetBoolArg("-proxynode", false);

    amIContactNode = gArgs.GetBoolArg("-contactnode", false);
    isGossipShutdown = gArgs.GetBoolArg("-gossipshutdown", false);
    shutdownTime = gArgs.GetArg("-shutdownTime", -1);

    generateTxNum = gArgs.GetArg("-generatetx", 0);
    generateTxTime = gArgs.GetArg("-timegeneratetx", 0);

    block_tx_num = gArgs.GetArg("-blocktxnum", 5);
    mining_time = gArgs.GetArg("-miningtime", 10);
    mining_time_dev = gArgs.GetArg("-miningtimedev", "2.0");


    std::cout << "Testing node up" << "\n";

    // init GlobalClock
    utility::globalclock_start = std::chrono::high_resolution_clock::now();

    /* init state machine.  */
    gStateMachine.InitStateMachine();

    // /* for testing pipe-input */
    // pthread_t tid;
    // pthread_create(&tid,NULL,writer_ABC,NULL);    


    /* initiate the state machine main loop */ 
    gStateMachine.StartStateMachine();


}
