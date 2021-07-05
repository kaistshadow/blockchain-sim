//
// Created by csrc on 21. 6. 6..
//

#include "POSStakes.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>

namespace libBLEEP_BL {
    void StakeList::updateInvertedMap() {
        inverted_leadermap.clear();
        totalStakedValue = 0;
        auto it = leadermap.begin();
        unsigned long value = 0;
        while (it != leadermap.end()) {
            inverted_leadermap.insert({value, it->first});
            value += it->second;
            it++;
        }
        fDirty = false;
        totalStakedValue = value;
    }

    void StakeList::addStake(unsigned long leader, unsigned long stakedValue) {
        auto it = leadermap.find(leader);
        if (it == leadermap.end()) {
            leadermap.insert({leader, stakedValue});
            std::cout<<"leadermap Successfully"<<"\n";
            std::cout<<"leader:"<<leader<<"|";
            std::cout<<"stakedValue:"<<stakedValue<<"\n";
        } else {
            it->second += stakedValue;
        }
        fDirty = true;
    }

    unsigned long StakeList::first() {
        if (fDirty) {
            updateInvertedMap();
        }
        return inverted_leadermap.begin()->second;
    }

    unsigned long StakeList::last() {
        if (fDirty) {
            updateInvertedMap();
        }
        return std::prev(inverted_leadermap.end())->second;
    }

    unsigned long StakeList::pickLeader(unsigned long v) {
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

    unsigned long StakeList::getTotal() {
        if (fDirty) {
            updateInvertedMap();
        }
        return totalStakedValue;
    }

    void StakeList::show_stake_peerlist() {
        std::cout<< "genesis stake setting ... " << "\n";
        for(auto iter = leadermap.begin(); iter != leadermap.end(); iter ++) {
            std::cout << iter->first << " " << iter->second << "\n";
        }
        std::cout << "----------------------------" << "\n";
    }

    void StakeList::load(std::string stakefile) {
        std::ifstream file(stakefile);
        if (!file.is_open()) {
            std::cout << "stake file not exists. abort.\n";
            exit(-1);
        }
        std::string str;
        while (std::getline(file, str)) {
            // trim
            str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
            str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
            str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
            str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
            size_t pivot = str.find(':');
            if (pivot == std::string::npos) {
                std::cout << "exception occured on stakefile parsing.\n";
                return;
            }
        
            std::string left = str.substr(0, pivot);
            std::string right = str.substr(pivot + 1);

            unsigned long leader;
            unsigned long stakedValue;
            try {
                leader = std::stoul(left);
                stakedValue = std::stoul(right);
                addStake(leader, stakedValue);
            }
            catch (const std::exception& expn) {
                std::cout << "exception occured on stakefile parsing.\n";
            }
        }
        return;
    }
}