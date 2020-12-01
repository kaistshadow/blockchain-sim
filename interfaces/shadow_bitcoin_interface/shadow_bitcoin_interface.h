#ifndef SHADOW_BITCOIN_INTERFACE_H
#define SHADOW_BITCOIN_INTERFACE_H

#include <string>

#ifdef __cplusplus
extern "C"
{
#endif

void shadow_bitcoin_register_hash(const char hash[]);
int shadow_bitcoin_check_hash(const char hash[]);

void update_log_map(const char prevblockhash[], const char blockhash[],const int txcount, const int height);
int get_tx_total_count(void);
int get_tx_count(const char blockhash[]);

#ifdef __cplusplus
}
#endif

#endif
