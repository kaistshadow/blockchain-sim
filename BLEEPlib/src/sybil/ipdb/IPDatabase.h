//
// Created by ilios on 21. 2. 23..
//

#ifndef BLEEP_IPDATABASE_H
#define BLEEP_IPDATABASE_H

#include <vector>
#include <string>

namespace libBLEEP_sybil {
    class IPDatabase {
    public:
        std::vector<std::pair<std::string, int>> &GetVReachableIP() { return _vReachableIP; };

        std::vector<std::string> &GetVUnreachableIP() { return _vUnreachableIP; }

        std::vector<std::string> &GetVAttackerIP() { return _vAttackerIP; }

        std::vector<std::string> &GetVShadowIP() { return _vShadowIP; }

        void SetVReachableIP(std::vector<std::string> vIP) {
            _vReachableIPonly = vIP;
        }

        std::vector<std::string> &GetVReachableIPOnly() {
            return _vReachableIPonly;
        }

    protected:
        std::vector<std::string> _vReachableIPonly;

        void InsertReachableIP(std::string ip, int uptimesec) {
            _vReachableIP.push_back({ip, uptimesec});
        }

        void InsertAttackerIP(std::string ip) {
            _vAttackerIP.push_back(ip);
        }

        std::vector<std::pair<std::string, int>> _vReachableIP;
        std::vector<std::string> _vUnreachableIP; // not used for this basic IPDatabase
        std::vector<std::string> _vAttackerIP; // malicious IPs
        std::vector<std::string> _vShadowIP; // not used for this basic IPDatabase
    };
}
#endif //BLEEP_IPDATABASE_H
