//
// Created by ilios on 21. 2. 23..
//

#ifndef BLEEP_IPDATABASE_H
#define BLEEP_IPDATABASE_H

#include <vector>
#include <string>
#include <map>
#include <set>

namespace sybiltest {
    class IPDatabase {
    public:
        std::vector<std::pair<std::string, std::vector<int>>> &GetIPDurationpair() { return _vIPDurationPair; };

        virtual void Initialize(int reachableIPNum, int unreachableIPNum, int shadowIPNum) = 0;

        std::vector<std::string> &GetVReachableIP() { return _vReachableIP; }

        std::vector<std::string> &GetVUnreachableIP() { return _vUnreachableIP; }

        std::vector<std::string> &GetVAttackerIP() { return _vAttackerIP; }

        std::vector<std::string> &GetVShadowIP() { return _vShadowIP; }

        std::map<std::string, std::vector<int>> &GetMIPDuration() { return _mIPDuration; }

        std::set<std::string> &GetSAliveIP() { return _sAliveIP; }
    protected:
        void InsertIPDurationPair(std::string ip, int uptime) {
            _vIPDurationPair.push_back({ip, {0, uptime}});
        }
        void InsertIPDurationPair(std::string ip, std::vector<int> duration) {
            _vIPDurationPair.push_back({ip, duration});
        }

        void InsertAttackerIP(std::string ip) {
            _vAttackerIP.push_back(ip);
        }

        std::vector<std::pair<std::string, std::vector<int>>> _vIPDurationPair;
        std::vector<std::string> _vReachableIP;
        std::map<std::string, std::vector<int>> _mIPDuration;
        std::vector<std::string> _vUnreachableIP; // not used for basic IPDatabase
        std::vector<std::string> _vAttackerIP; // malicious IPs
        std::vector<std::string> _vShadowIP; // not used for basic IPDatabase
        std::set<std::string> _sAliveIP; // used for managing alive IP
    };
}
#endif //BLEEP_IPDATABASE_H
