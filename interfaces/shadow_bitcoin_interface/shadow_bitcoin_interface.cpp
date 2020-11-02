#include "shadow_bitcoin_interface.h"

#define COINFLIP_BLK_TRACKER 13
#include <unordered_set>
#include <shadow_interface.h>

std::unordered_set<std::string> * _get_blk_tracker() {
    static std::unordered_set<std::string> * pt = (std::unordered_set<std::string> *)shadow_claim_shared_entry(new std::unordered_set<std::string>(), sizeof(std::unordered_set<std::string>), COINFLIP_BLK_TRACKER);
    return pt;
}

void shadow_bitcoin_register_hash(std::string hash) {
    std::unordered_set<std::string> * _blk_tracker = _get_blk_tracker();
    _blk_tracker->insert(hash);
    return;
}
bool shadow_bitcoin_check_hash(std::string hash) {
    std::unordered_set<std::string> * _blk_tracker = _get_blk_tracker();
    std::unordered_set<std::string>::const_iterator got = _blk_tracker->find(hash);
    if (got == _blk_tracker->end()) {
        return false;
    }
    return true;
}