#include "shadow_interface.h"
#include <stdio.h>

// plugin -> shadow
int puts_temp(const char *str) {
    puts("local puts_temp:");
    return puts(str);
}


int shadow_pipe2(int pipefds[2], int flags) {
    printf("local-shadow_pipe2 called\n");
    return pipe2(pipefds, flags);
}

int shadow_push_eventlog(const char *str) {
    // do nothing
    printf("shadow_push_eventlog is not properly executed for event:%s\n", str);
    return 0;
}
