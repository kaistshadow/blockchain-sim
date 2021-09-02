// "Copyright [2021] <kaistshadow>"

//
// Created by ilios on 21. 2. 15..
//

#ifndef BLEEPEVAL_SYBILTEST_LIBRARY_SRC_IPDB_SIMPLEIPDATABASE_H_
#define BLEEPEVAL_SYBILTEST_LIBRARY_SRC_IPDB_SIMPLEIPDATABASE_H_

#include <vector>
#include <string>
#include "IPDatabase.h"

namespace sybiltest {
class SimpleIPDatabase : public IPDatabase {
 public:
    SimpleIPDatabase() {
        // 1. initiate IP database
        InsertIPDurationPair("1.0.0.2", 100);
        InsertIPDurationPair("1.0.0.3", 100);
        InsertIPDurationPair("1.0.0.4", 100);
        InsertIPDurationPair("1.0.0.5", 100);
        InsertIPDurationPair("1.0.0.6", 100);
        InsertIPDurationPair("1.0.0.7", 100);
        InsertIPDurationPair("1.0.0.8", 100);
        InsertIPDurationPair("1.0.0.9", 200);
        InsertIPDurationPair("1.0.0.10", 300);
        InsertIPDurationPair("1.0.0.11", 400);

        // 2. insert shadow IP into the database
        InsertAttackerIP("2.0.0.1");
        InsertAttackerIP("2.0.0.2");
        InsertAttackerIP("2.0.0.3");
        InsertAttackerIP("2.0.0.4");
        InsertAttackerIP("2.0.0.5");
        InsertAttackerIP("2.0.0.6");
        InsertAttackerIP("2.0.0.7");
        InsertAttackerIP("2.0.0.8");
        InsertAttackerIP("2.0.0.9");
        InsertAttackerIP("2.0.0.10");
    }

    void Initialize(int reachableIPNum, int unreachableIPNum, int shadowIPNum) {
        // not used interface for this simple database
    }
};
}  // namespace sybiltest

#endif  //  BLEEPEVAL_SYBILTEST_LIBRARY_SRC_IPDB_SIMPLEIPDATABASE_H_
