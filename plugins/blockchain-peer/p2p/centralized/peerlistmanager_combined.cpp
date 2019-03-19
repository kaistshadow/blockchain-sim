#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <time.h>

#include "peerlistmanager_combined.h"

PeerListManagerCombined* PeerListManagerCombined::instance = 0;

PeerListManagerCombined* PeerListManagerCombined::GetInstance() {
    if (instance == 0) {
        instance = new PeerListManagerCombined();
    }
    return instance;
}

// InitializePeerList : make outPeer for given hostnames
// void PeerListManagerCombined::InitializeDomainList(int num, char *hostnames[]) {
//     for (int i = 0; i < num; i++) {
//         std::cout << "Add domainname : " << hostnames[i] << "\n";        
//         domains.push_back(std::string(hostnames[i]));
//     }
// }

// std::string PeerListManagerCombined::GetDomainFromIp(std::string ip) {
//     for (std::string domain : domains) {
//         struct addrinfo* servinfo;
//         if (getaddrinfo(domain.c_str(), NULL, NULL, &servinfo) == 0) {
//             std::string ipaddr = std::string(inet_ntoa(((struct sockaddr_in*) (servinfo->ai_addr))->sin_addr));
//             if (ip == ipaddr) {
//                 return domain;
//             }
//         }
//     }

//     return "";
// }

PeerCombined *PeerListManagerCombined::GetPeerByDomain(std::string domain) {
    for (auto peer : peerList) {
        if (peer->hostname == domain) 
            return peer;
    }
    return nullptr;
}

