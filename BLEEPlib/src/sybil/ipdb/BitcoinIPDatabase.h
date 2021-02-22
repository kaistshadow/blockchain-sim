//
// Created by ilios on 21. 2. 15..
//

#ifndef BLEEP_IPDATABASE_H
#define BLEEP_IPDATABASE_H

#include <vector>
#include <string>

namespace libBLEEP_sybil {
    class BitcoinIPDatabase {
    public:
        BitcoinIPDatabase() {
            std::ifstream read("churn.txt");
            std::regex re("\\d+\\.\\d+\\.\\d+\\.\\d+");

        }

        std::vector<std::pair<std::string, int>> &GetVReachableIP() { return _vReachableIP; };

        std::vector<std::string> &GetVShadowIP() { return _vShadowIP; }

    private:
        void InsertReachableIP(std::string, int);

        void InsertShadowIP(std::string);

        std::vector<std::pair<std::string, int>> _vReachableIP;
        std::vector<std::string> _vUnreachableIP;
        std::vector<std::string> _vShadowIP; // malicious IPs

    };
}

#endif //BLEEP_IPDATABASE_H
