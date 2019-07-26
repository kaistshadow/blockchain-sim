#ifndef SHADOW_INTERFACE_H
#define SHADOW_INTERFACE_H

#include <unistd.h>
#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif

int puts_temp(const char *str);

int shadow_pipe2(int pipefds[2], int flags);

int shadow_push_eventlog(const char *str);

int shadow_usleep(useconds_t usec);

int shadow_clock_gettime(clockid_t clk_id, struct timespec *tp);

int init_shadow_clock_update();
int next_shadow_clock_update(const char* prefix);

#ifdef __cplusplus
}
#endif






#endif
