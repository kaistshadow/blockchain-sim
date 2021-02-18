//
// Created by ilios on 21. 2. 15..
//

#ifndef BLEEP_IPDATABASE_H
#define BLEEP_IPDATABASE_H

#include <vector>
#include <string>

namespace libBLEEP_sybil {
    class IPDatabase {
    public:
        IPDatabase();

        std::vector<std::pair<std::string, int>> &GetVReachableIP() { return _vReachableIP; };
    private:
        void InsertReachableIP(std::string, int);

        std::vector<std::pair<std::string, int>> _vReachableIP;

    };
}

#endif //BLEEP_IPDATABASE_H
