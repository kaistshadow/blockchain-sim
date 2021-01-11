#include "shadow_bitcoin_interface.h"

#include <iostream>
#include <unordered_set>

std::unordered_set<std::string> t;

void shadow_bitcoin_register_hash(const char hash[]) {
    t.insert(hash);
    return;
}
int shadow_bitcoin_check_hash(const char hash[]) {
    std::unordered_set<std::string>::const_iterator got = t.find(hash);
    int res = (got != t.end());
    return res;
}

void update_log_map(const char prevblockhash[], const char blockhash[],const int txcount, const int height) {
    printf("update_log_map in shadow_bitcoin_interface.c\n");
}

int get_tx_total_count(void){
    printf("get_total_count in shadow_bitcoin_interface.c\n");
    return 0;
}
int get_tx_count(const char blockhash[]){
    printf("get_tx_count in shadow_bitcoin_interface.c\n");
    return 0;
}