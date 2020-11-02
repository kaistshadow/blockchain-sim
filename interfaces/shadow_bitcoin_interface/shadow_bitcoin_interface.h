#ifndef SHADOW_BITCOIN_INTERFACE_H
#define SHADOW_BITCOIN_INTERFACE_H

#include <string>

void shadow_bitcoin_register_hash(std::string hash);
bool shadow_bitcoin_check_hash(std::string hash);

#endif
