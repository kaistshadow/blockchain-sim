//
// Created by csrc on 21. 6. 6..
//

#include "POSStakes.h"

#include <fstream>

namespace libBLEEP_BL {
    void StakeList::updateInvertedMap() {
        inverted_leadermap.clear();
        totalStakedValue = 0;
        auto it = leadermap.begin();
        unsigned int value = 0;
        while (it != leadermap.end()) {
            inverted_leadermap.insert({value, it->first});
            value += it->second;
            it++;
        }
        fDirty = false;
        totalStakedValue = value;
    }

    void StakeList::addStake(unsigned int leader, unsigned int stakedValue) {
        auto it = leadermap.find(leader);
        if (it == leadermap.end()) {
            leadermap.insert({leader, stakedValue});
        } else {
            it->second += stakedValue;
        }
        fDirty = true;
    }

    unsigned int StakeList::first() {
        if (fDirty) {
            updateInvertedMap();
        }
        return inverted_leadermap.begin()->second;
    }

    unsigned int StakeList::last() {
        if (fDirty) {
            updateInvertedMap();
        }
        return std::prev(inverted_leadermap.end())->second;
    }

    unsigned int StakeList::pickLeader(unsigned int v) {
        if (fDirty) {
            updateInvertedMap();
        }
        auto it = inverted_leadermap.upper_bound(v);
        if (it == inverted_leadermap.end()) {
            return last();
        } else {
            return std::prev(it)->second;
        }
    }

    unsigned int StakeList::getTotal() {
        if (fDirty) {
            updateInvertedMap();
        }
        return totalStakedValue;
    }

    void StakeList::load(std::string stakefile) {
        std::ifstream file(stakefile);
        // TODO: assert existence
        std::string str;
        while (std::getline(file, str)) {
            // TODO: trim
            // TODO: separate number with ':'
            // TODO: assert each number (unsigned int, double)
            // TODO: add
        }
    }
}