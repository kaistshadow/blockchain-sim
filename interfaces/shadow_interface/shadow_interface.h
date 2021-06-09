#ifndef SHADOW_INTERFACE_H
#define SHADOW_INTERFACE_H

#include <unistd.h>
#include <time.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C"
{
#endif

int puts_temp(const char *str);

// adaptive IP&host matching
int shadow_bind(int fd, const struct sockaddr *addr, socklen_t len);
int shadow_register_NIC(const struct sockaddr *addr, socklen_t len);

int shadow_pipe2(int pipefds[2], int flags);

int shadow_push_eventlog(const char *str);

int shadow_usleep(useconds_t usec);

int shadow_clock_gettime(clockid_t clk_id, struct timespec *tp);

int init_shadow_clock_update();
int next_shadow_clock_update(const char* prefix);

// virtual id assignment for plugins
int shadow_assign_virtual_id();

void* shadow_claim_shared_entry(void* ptr, size_t sz, int shared_id);

#ifdef __cplusplus
}
#endif


#endif
