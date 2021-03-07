//
// Created by ilios on 21. 2. 15..
//

#ifndef BLEEP_BITCOIN_IPDATABASE_H
#define BLEEP_BITCOIN_IPDATABASE_H

#include <vector>
#include <string>
#include <set>
#include <regex>
#include <fstream>
#include <arpa/inet.h>
#include <assert.h>
#include <iostream>

#include "IPDatabase.h"

namespace sybiltest {
    class BitcoinIPDatabase : public IPDatabase {
    public:
        BitcoinIPDatabase() {
            // Load and store IP address database from churn.txt
            std::ifstream read("churn.txt");
            if (read.fail()) {
                std::cout << "failed to read churn.txt file" << "\n";
                exit(-1);
            }
            std::regex re("\\d+\\.\\d+\\.\\d+\\.\\d+");
            for (std::string line; std::getline(read, line);) {
                auto pos = line.find(' ');
                std::string ip = line.substr(0, pos);
                int duration = std::stoi(line.substr(pos));

                if (duration > 0 && std::regex_match(ip, re)) {
                    InsertIPDurationPair(ip, duration);
                    _mIPDuration[ip] = duration;
                }
            }
        }
        void Initialize(int reachableIPNum, int unreachableIPNum, int shadowIPNum) {
            std::set<std::string> sLegiIP;
            std::set<std::string> sUnreachLegiIP;
            std::set<std::string> sShadowIP;

            int reachableIPCount = 0;
            for (auto&[ip, duration] : _vIPDurationPair) {
                sLegiIP.insert(ip);
                reachableIPCount++;
                if (reachableIPCount == reachableIPNum)
                    break;
            }

            if (reachableIPCount < reachableIPNum) {
                std::cout << "not enough number of reachable IP" << "\n";
                exit(-1);
            }

            // Store unreachable IP address database
            // Collect unreachable non-duplicated legitimate IPs
            for (int i = 0; i < unreachableIPNum; i++) {
                while (true) {
                    std::string randIP = _generateRandomIP();
                    if (!sLegiIP.contains(randIP) && !sUnreachLegiIP.contains(randIP)) {
                        sUnreachLegiIP.insert(randIP);
                        break;
                    }
                }
            }

            // Collect shadow IPs
            for (int i = 0; i < shadowIPNum; i++) {
                while (true) {
                    std::string randIP = _generateRandomIP();
                    if (!sLegiIP.contains(randIP) && !sUnreachLegiIP.contains(randIP) && !sShadowIP.contains(randIP)) {
                        sShadowIP.insert(randIP);
                        break;
                    }
                }
            }

            _vReachableIP = std::vector<std::string>(sLegiIP.begin(), sLegiIP.end());
            _vUnreachableIP = std::vector<std::string>(sUnreachLegiIP.begin(), sUnreachLegiIP.end());
            _vShadowIP = std::vector<std::string>(sShadowIP.begin(), sShadowIP.end());

            while (true) {
                std::string randIP = _generateRandomIP();
                if (!sLegiIP.contains(randIP) && !sUnreachLegiIP.contains(randIP) && !sShadowIP.contains(randIP)) {
                    _vAttackerIP.push_back(randIP);
                    break;
                }
            }
        }

    private:
        // utility function
        std::string _generateRandomIP() {
            std::stringstream ss;
            while (true) {
                int ip[4];
                for (int i = 0; i < 4; i++) {
                    ip[i] = rand() % 256;
                    if (ip[i] == 0)
                        ip[i]++;
                }

                if (ip[0] == 10)
                    continue;
                if (ip[0] == 172 && ip[1] == 16)
                    continue;
                if (ip[0] == 192 && ip[1] == 168)
                    continue;

                ss.str("");
                ss << ip[0] << "." << ip[1] << "." << ip[2] << "." << ip[3];

                if (!_dns_isRestricted(ss.str()))
                    break;
            }
            return ss.str();
        }

        bool _dns_isRestricted(std::string ip) {
            struct in_addr netIP;
            inet_pton(AF_INET, ip.c_str(), &netIP);
            /* http://en.wikipedia.org/wiki/Reserved_IP_addresses#Reserved_IPv4_addresses */
            if (_dns_isIPInRange(netIP, "0.0.0.0/8") ||
                _dns_isIPInRange(netIP, "10.0.0.0/8") ||
                _dns_isIPInRange(netIP, "100.64.0.0/10") ||
                _dns_isIPInRange(netIP, "127.0.0.0/8") ||
                _dns_isIPInRange(netIP, "169.254.0.0/16") ||
                _dns_isIPInRange(netIP, "172.16.0.0/12") ||
                _dns_isIPInRange(netIP, "192.0.0.0/29") ||
                _dns_isIPInRange(netIP, "192.0.2.0/24") ||
                _dns_isIPInRange(netIP, "192.88.99.0/24") ||
                _dns_isIPInRange(netIP, "192.168.0.0/16") ||
                _dns_isIPInRange(netIP, "198.18.0.0/15") ||
                _dns_isIPInRange(netIP, "198.51.100.0/24") ||
                _dns_isIPInRange(netIP, "203.0.113.0/24") ||
                _dns_isIPInRange(netIP, "224.0.0.0/4") ||
                _dns_isIPInRange(netIP, "240.0.0.0/4") ||
                _dns_isIPInRange(netIP, "255.255.255.255/32")) {
                return true;
            } else {
                return false;
            }
        }

        bool _dns_isIPInRange(const struct in_addr netIP, std::string cidrStr) {
            auto pos = cidrStr.find('/');
            if (pos == std::string::npos)
                assert (0 && "wrong argument for _dns_isIPInRange");
            std::string cidrIPStr = cidrStr.substr(0, pos);
            std::string bits = cidrStr.substr(pos + 1);
            int cidrBits = std::stoi(bits);

            assert(cidrBits >= 0 && cidrBits <= 32);

            /* first create the mask in host order */
            in_addr_t netmask = 0;
            for (int i = 0; i < 32; i++) {
                /* move one so LSB is 0 */
                netmask = netmask << 1;
                if (cidrBits > i) {
                    /* flip the LSB */
                    netmask++;
                }
            }

            /* flip to network order */
            netmask = htonl(netmask);

            /* get the subnet ip in network order */
            struct in_addr subnetIP;
            inet_pton(AF_INET, cidrIPStr.c_str(), &subnetIP);

            /* all non-subnet bits should be flipped */
            if ((netIP.s_addr & netmask) == (subnetIP.s_addr & netmask)) {
                return true;
            } else {
                return false;
            }
        }


    };
}

#endif //BLEEP_BITCOIN_IPDATABASE_H
