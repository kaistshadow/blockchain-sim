#include "shadow_bitcoin_interface.h"

#include <iostream>
#include <unordered_set>

std::unordered_set<std::string> t;

void shadow_bitcoin_register_hash(const char hash[],int reindex) {
    t.insert(hash);
    return;
}
int shadow_bitcoin_check_hash(const char hash[]) {
    std::unordered_set<std::string>::const_iterator got = t.find(hash);
    int res = (got != t.end());
    return res;
}

void shadow_bitcoin_load_hash(){
  printf("start shadow_bitcoin_load_hash\n");
}
