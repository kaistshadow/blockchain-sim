#include <iostream>
#include <memory>
#include "Configuration.h"
#include "utility/NodeInfo.h"
#include "utility/ArgsManager.h"
#include "utility/GlobalClock.h"
#include "statemachine/StateMachine.h"

// =================  Blockchain Node Configuration Start ===================

// general configuration

// configurations for proxy-based network

// configurations for gossip-based network

// Blockchain Consensus Configuration
int block_tx_num = 5;

// Blockchain Transaction Configuration

//This function continously writes Capital Alphabet into fd[1]
//Waits if no more space is available

// void *writer_ABC(void*)
// {
//     int     result;
//     char    ch='A';

//     int pipewriteFD = gStateMachine.pipeManager.GetPipe(gStateMachine.testPipeID)->GetWritePipeFD();
//     while(1){
//         result = write (pipewriteFD, &ch,1);
//         if (result != 1){
//             perror ("write");
//             exit (2);
//         }

//         printf ("Writer_ABC: %c\n", ch);
//         break;
//         if(ch == 'Z')
//             break;
//             // ch = 'A'-1;

//         ch++;
//     }
//     close(pipewriteFD);
//     close(gStateMachine.pipeManager.GetPipe(gStateMachine.testPipeID)->GetReadPipeFD());
//     printf("socket closed!\n");
//     return 0;
// }

int main(int argc, char *argv[]) {
    //////////////////////////////////
    // argument parsing and setting //
    //////////////////////////////////
    gArgs.ParseParameters(argc, argv);

    if (gArgs.IsArgSet("-?") || gArgs.IsArgSet("-h") ||  gArgs.IsArgSet("-help") || gArgs.IsArgSet("-version")) {
        std::string strUsage = gArgs.HelpMessage();
        std::cout << strUsage << "\n";
        return 0;
    }
    block_tx_num = gArgs.GetArg("-blocktxnum", 5);





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
