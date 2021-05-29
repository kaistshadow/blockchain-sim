//
// Created by csrc on 21. 4. 26..
//

#include "shadow_memshare_interface.h"

#include <iostream>

int shadow_check_memshare_flag() {
    return 0;
}
void shadow_try_register_memshare_table(void* type_idx_ref, void* mtbl) {
    std::cout<<"local-shadow_try_register_memshare_table\n";
    return;
}
void shadow_memshare_try_share(void* type_idx_ref, void* sptr_ref) {
    std::cout<<"local-shadow_memshare_try_share\n";
}
void* shadow_memshare_lookup(void* type_idx_ref, void* sptr_ref) {
    std::cout<<"local-shadow_memshare_lookup\n";
    return sptr_ref;
}

std::unordered_set<const char*>* type_shared_checks = new std::unordered_set<const char*>();
void set_shared_type_cache(std::type_index tidx) {
    type_shared_checks->insert(tidx.name());
}
int check_shared_type_cache(std::type_index tidx) {
    return (type_shared_checks->find(tidx.name()) == type_shared_checks->end()?0:1);
}

int memshare::get_memshare_flag() {
    static int memshare_flag = shadow_check_memshare_flag();
    return memshare_flag;
}