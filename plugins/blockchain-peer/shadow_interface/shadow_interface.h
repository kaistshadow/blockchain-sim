#ifndef SHADOW_INTERFACE_H
#define SHADOW_INTERFACE_H

#ifdef __cplusplus
extern "C"
{
#endif

int puts_temp(const char *str);

int shadow_pipe2(int pipefds[2], int flags);

int shadow_push_eventlog(const char *str);

#ifdef __cplusplus
}
#endif






#endif
