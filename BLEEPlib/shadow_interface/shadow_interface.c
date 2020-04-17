#include "shadow_interface.h"
#include <stdio.h>

struct timespec clock_start;

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
    /* printf("shadow_push_eventlog is ignored for event:%s\n", str); */
    return 0;
}

int shadow_usleep(useconds_t usec) {
    // do nothing
    /* printf("shadow_usleep is ignored\n"); */
    return 0;
}

int shadow_clock_gettime(clockid_t clk_id, struct timespec *tp) {
    // do real clock_gettime
    return clock_gettime(clk_id, tp);
}

int init_shadow_clock_update() {
    return shadow_clock_gettime(CLOCK_MONOTONIC, &clock_start);
}
int next_shadow_clock_update(const char* prefix) {
    struct timespec cur;
    shadow_clock_gettime(CLOCK_MONOTONIC, &cur);
    double nanoseconds = cur.tv_nsec >= clock_start.tv_nsec
                        ? (cur.tv_nsec - clock_start.tv_nsec) + (cur.tv_sec - clock_start.tv_sec) * 1e9
                        : (clock_start.tv_nsec - cur.tv_nsec) + (cur.tv_sec - clock_start.tv_sec - 1) * 1e9;
    shadow_usleep(nanoseconds / 1e3);
    /* printf("%s:%lu\n", prefix, (unsigned long)nanoseconds); */
    /* double milliseconds = cur.tv_nsec >= clock_start.tv_nsec */
    /*                     ? (cur.tv_nsec - clock_start.tv_nsec) / 1e6 + (cur.tv_sec - clock_start.tv_sec) * 1e3 */
    /*                     : (clock_start.tv_nsec - cur.tv_nsec) / 1e6 + (cur.tv_sec - clock_start.tv_sec - 1) * 1e3; */
    /* shadow_usleep(milliseconds * 1000); */
    clock_start = cur;
    return nanoseconds;
}

int shadow_assign_virtual_id() {
    printf("local-shadow_assign_virtual_id called\n");
    return 0;
}