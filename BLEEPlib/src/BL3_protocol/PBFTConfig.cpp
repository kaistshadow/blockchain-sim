#include "PBFTConfig.h"

#include <fstream>
#include <iostream>
#include <algorithm>

namespace libBLEEP_BL {
    void PBFTConfig::load(std::string filename, unsigned long selfExclude) {
        std::ifstream file(filename);
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

            unsigned long pubkey;
            try {
                pubkey = std::stoul(str);
                // exclude my pubkey
                if (selfExclude == pubkey) {
                    continue;
                }
                // register pubkey without fd connection
                consensusMap.insert({pubkey, ""});
            }
            catch (const std::exception& expn) {
                std::cout << "exception occured on stakefile parsing.\n";
            }
        }
    }
    void PBFTConfig::assignSource(unsigned long pubkey, std::string peer) {
        std::cout << "Debug assignSource: " << pubkey << " to " << peer << "\n";
        auto it = consensusMap.find(pubkey);
        if (it == consensusMap.end()) {
            std::cout << "Cannot find pubkey\n";
            return;
        }
        if (it->second == "") {
            std::cout << "Debug " << pubkey << " is newly assigned to " << peer << "\n";
            connectedCount++;
        }
        it->second = peer;

        auto inverted_it = peerPubkeyMap.find(peer);
        if (inverted_it == peerPubkeyMap.end()) {
            peerPubkeyMap.insert({peer, pubkey});
        } else {
            inverted_it->second = pubkey;
        }
    }
    std::string PBFTConfig::getPubkeyPeer(unsigned long pubkey) {
        auto it = consensusMap.find(pubkey);
        if (it == consensusMap.end()) {
            std::cout << "Cannot find pubkey\n";
            return "";
        }
        return it->second;
    }
    int PBFTConfig::getPeerPubkey(std::string peer, unsigned long& res) {
        auto it = peerPubkeyMap.find(peer);
        if (it == peerPubkeyMap.end()) {
            std::cout << "Cannot find pubkey\n";
            return -1;
        }
        res = it->second;
        return 0;
    }
    bool PBFTConfig::isAllConnected() {
        std::cout << "Debug consensus map size: " << consensusMap.size() << ", connectedCount: " << connectedCount << "\n";
        return consensusMap.size() == connectedCount;
    }
    unsigned int PBFTConfig::size() {
        return consensusMap.size();
    }
}