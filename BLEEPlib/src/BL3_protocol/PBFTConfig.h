//
// Created by Hyunjin Kim on 21. 6. 14..
//

#ifndef BLEEP_PBFTCONFIG_H
#define BLEEP_PBFTCONFIG_H

#include <map>

namespace libBLEEP_BL {
    class PBFTConfig {
        // map between consensus id and actual source file descriptor
        std::map<unsigned long, std::string> consensusMap;
        std::map<std::string, unsigned long> peerPubkeyMap;
        unsigned int connectedCount = 0;
    public:
        void load(std::string filename, unsigned long selfExclude);
        void assignSource(unsigned long pubkey, std::string peer);
        std::string getPubkeyPeer(unsigned long pubkey);
        int getPeerPubkey(std::string peer, unsigned long& res);
        bool isAllConnected();
        unsigned int size();
    };
}

#endif //BLEEP_PBFTCONFIG_H
