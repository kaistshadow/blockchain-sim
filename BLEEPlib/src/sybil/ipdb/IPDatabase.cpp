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
    InsertReachableIP("1.0.0.9", 200);
    InsertReachableIP("1.0.0.10", 300);
    InsertReachableIP("1.0.0.11", 400);

    // 2. insert shadow IP into the database
    InsertShadowIP("2.0.0.1");
    InsertShadowIP("2.0.0.2");
    InsertShadowIP("2.0.0.3");
    InsertShadowIP("2.0.0.4");
    InsertShadowIP("2.0.0.5");
    InsertShadowIP("2.0.0.6");
    InsertShadowIP("2.0.0.7");
    InsertShadowIP("2.0.0.8");
    InsertShadowIP("2.0.0.9");
    InsertShadowIP("2.0.0.10");

}

void IPDatabase::InsertReachableIP(std::string ip, int uptimesec) {
    _vReachableIP.push_back({ip, uptimesec});
}

void IPDatabase::InsertShadowIP(std::string ip) {
    _vShadowIP.push_back(ip);
}