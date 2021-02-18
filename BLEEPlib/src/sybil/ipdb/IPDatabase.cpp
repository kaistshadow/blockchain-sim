//
// Created by ilios on 21. 2. 15..
//

#include "IPDatabase.h"

using namespace libBLEEP_sybil;

IPDatabase::IPDatabase() {
    // 1. initiate IP database
    InsertReachableIP("1.0.0.2", 100);
    InsertReachableIP("1.0.0.3", 100);
    InsertReachableIP("1.0.0.4", 100);
    InsertReachableIP("1.0.0.5", 100);
    InsertReachableIP("1.0.0.6", 100);
    InsertReachableIP("1.0.0.7", 100);
    InsertReachableIP("1.0.0.8", 100);
    InsertReachableIP("1.0.0.9", 100);
    InsertReachableIP("1.0.0.10", 100);
    InsertReachableIP("1.0.0.11", 100);

}

void IPDatabase::InsertReachableIP(std::string ip, int uptimesec) {
    _vReachableIP.push_back({ip, uptimesec});
}