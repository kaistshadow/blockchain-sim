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
            std::ifstream read("../churn_8333.txt");
            if (read.fail()) {
                std::cout << "failed to read churn.txt file" << "\n";
                exit(-1);
            }
            std::regex re("\\d+\\.\\d+\\.\\d+\\.\\d+:8333");
            //int mintime = 1541030644; // current minimum is 2018-11/1541030644.json
            /*
             * 0.19.0 release date: 2019-11-24
             * use next date of release date as the earliest simulation date
             *
             * 2019-11-25: 1577199600
             * 2021-07-01: 1627743600
             *
             * a = 1577199600
             * b = 1627743600
             * list(range(a, b, (b-a)//10))
             * >> [1577199600, 1582254000, 1587308400, 1592362800, 1597417200, 1602471600, 1607526000, 1612580400, 1617634800, 1622689200]
             */
            int begintime = 1577199600; // 2018-01-01: 1517410800, minimum time in 2018-11: 1541030644
            for (std::string line; std::getline(read, line);) {
                auto pos = line.find(' ');
                /*
                // Previous version of churn data
                std::string ip = line.substr(0, pos);
                int duration = std::stoi(line.substr(pos));

                if (duration > 0 && std::regex_match(ip, re)) {
                    InsertIPDurationPair(ip, {0, duration});
                    _mIPDuration[ip] = {0, duration};
                }
                 */
                std::string ipport = line.substr(0, pos);
                if(!std::regex_match(ipport, re)) {
                    // Only supports IPv4 with port 8333
                    continue;
                }
                auto delimiter = ipport.find(':');
                std::string ip = ipport.substr(0, delimiter);
                int port = stoi(ipport.substr(delimiter+1));
                vector<int> duration;
                std::string timestamp;

                /*
                // For testing, all nodes churned in at beginning, and churned out after one week
                duration.push_back(0);
                duration.push_back(60 * 60 * 24 * 7);
                InsertIPDurationPair(ip, duration);
                _mIPDuration[ip] = duration;
                */

                bool alive = false; // indicating if the node is alive
                bool firstchurn = true; // indicating if first timestamp has been pushed to duration vector
                // setting small value can cut off churn data, boosting up testing
                // to test with whole data, set this value at zero or negative value
                int maxchurn = 0;
                std::stringstream ss = std::stringstream(line.substr(pos+1));
                while(std::getline(ss, timestamp, ' ')) {
                    if(firstchurn && stoi(timestamp) >= begintime) {
                        // first timestamp to push in
                        // if alive, the node should be churned out at this timestamp,
                        // so churn in at the beginning of the simulation
                        if(alive) duration.push_back(0);
                        firstchurn = !firstchurn;
                    }
                    // After handling first churn in, just push timestamp into duration
                    if (!firstchurn) {
                        assert(stoi(timestamp) >= begintime);
                        duration.push_back(stoi(timestamp) - begintime);
                        if(--maxchurn == 0) break;
                    }
                    alive = !alive;
                }
                if (duration.size() > 0) {
                    InsertIPDurationPair(ip, duration);
                    _mIPDuration[ip] = duration;
                }
            }
            std::cout<<"parsed benign ip size : "<<_mIPDuration.size()<<"\n";
        }
        void Initialize(int reachableIPNum, int unreachableIPNum, int shadowIPNum) {
            std::set<std::string> sLegiIP;
            std::set<std::string> sUnreachLegiIP;
            std::set<std::string> sShadowIP;

            int reachableIPCount = 0;
            for (auto&[ip, duration] : _vIPDurationPair) {
                sLegiIP.insert(ip);
                reachableIPCount++;
                //if (reachableIPCount == reachableIPNum)
                //    break;
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
                    if (sLegiIP.find(randIP) == sLegiIP.end() && sUnreachLegiIP.find(randIP) == sUnreachLegiIP.end()) {
                        sUnreachLegiIP.insert(randIP);
                        break;
                    }
                }
            }

            // Collect shadow IPs
            std::cout<< "shadow IP collecting begin\n";
            for (int i = 0; i < shadowIPNum; i++) {
                while (true) {
                    std::string randIP = _generateRandomIP();
                    if (sLegiIP.find(randIP) == sLegiIP.end() && sUnreachLegiIP.find(randIP) == sUnreachLegiIP.end() &&
                        sShadowIP.find(randIP) == sShadowIP.end()) {
                        sShadowIP.insert(randIP);
                        std::cout<<randIP<<"\n";
                        break;
                    }
                }
            }
            std::cout<< "shadow IP collecting done\n";

            _vReachableIP = std::vector<std::string>(sLegiIP.begin(), sLegiIP.end());
            _vUnreachableIP = std::vector<std::string>(sUnreachLegiIP.begin(), sUnreachLegiIP.end());
            _vShadowIP = std::vector<std::string>(sShadowIP.begin(), sShadowIP.end());
            int attackerNum = 100;
            while (true) {
                std::string randIP = _generateRandomIP();
                if (sLegiIP.find(randIP) == sLegiIP.end() && sUnreachLegiIP.find(randIP) == sUnreachLegiIP.end() &&
                    sShadowIP.find(randIP) == sShadowIP.end()) {
                    _vAttackerIP.push_back(randIP);
                    attackerNum--;
                    if(attackerNum == 0)break;
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
