#ifndef SHADOW_BITCOIN_INTERFACE_H
#define SHADOW_BITCOIN_INTERFACE_H

#include <string>

#ifdef __cplusplus
extern "C"
{
#endif

void shadow_bitcoin_register_hash(const char hash[]);
int shadow_bitcoin_check_hash(const char hash[]);

#ifdef __cplusplus
}
#endif

#endif
