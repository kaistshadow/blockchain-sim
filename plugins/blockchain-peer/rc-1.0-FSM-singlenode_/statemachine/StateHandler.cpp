#include "StateHandler.h"

#include <ev.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

StateEnum idleStateHandler() {
    std::cout << "idle state handler executed!" << "\n";

    if (!idleState.IsInitialized()) {
        std::cout << "idle state is not properly initialized!" << "\n";
        exit(1);
    }

    // wait for next event
    struct ev_loop *loop = EV_DEFAULT;
    // ev_run (loop);
    ev_run (loop, EVRUN_ONCE);

    printf("ev_run returned\n");

    StateEnum nextState;


    switch (idleState.GetEventType()) {
    case LibevEventEnum::none:
        printf("Error! no event triggered but ev_run is returned!\n");
        nextState = StateEnum::exitState;
        break;
    case LibevEventEnum::pipeReadEvent:
        int fd = idleState.GetEventTriggeredFD();
        char string_read[2000];
        memset(string_read, 0, 2000);
        int n;
        n = read(fd, string_read, 2000);
        if (n == 0) {
            printf("read done : n=%d, string_read=%s\n", n, string_read);
            nextState = StateEnum::exitState;
        }
        else if (n > 0 && n <= 2000) {
            printf("read : n=%d, string_read=%s\n", n, string_read);
            printf("read should be continued!\n");
            nextState = StateEnum::idleState;
        }
        else {
            perror("read error");
            exit(-1);
        }
        break;
    } 

    return nextState;
}
StateEnum exitStateHandler() {
    std::cout << "exit state handler executed!" << "\n";
    return StateEnum::exitState;
}

