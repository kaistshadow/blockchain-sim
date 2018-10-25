#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <time.h>

#include "peerlistmanager.h"


SimplePeerListManager* SimplePeerListManager::instance = 0;

SimplePeerListManager* SimplePeerListManager::GetInstance() {
    if (instance == 0) {
        instance = new SimplePeerListManager();
    }
    return instance;
}


// TODO : Implement more flexible peerlist setup process
// For example, receiving peer ip list from command line interface
// But, How to dynamically change the peerlist after initialization? 
// Check out how real bitcoin client handles this problem.

// InitializePeerList : make outPeer for given hostnames
void SimplePeerListManager::InitializeOutPeerList(int num_outpeer, char *hostname_outpeers[]) {
    for (int i = 0; i < num_outpeer; i++) {
        std::cout << "Initialize outPeer : " << hostname_outpeers[i] << "\n";        
        Peer* outPeer = new Peer(std::string(hostname_outpeers[i]));
        outPeerList.push_back(outPeer);
    }
}


std::string SimplePeerListManager::GetDomainFromIp(std::string ip) {
    for (auto peer : inPeerList) {
        if (peer->ipaddr == ip && peer->hostname != "")
            return peer->hostname;
    }
    for (auto peer : outPeerList) {
        if (peer->ipaddr == ip && peer->hostname != "")
            return peer->hostname;
    }
    return "";
}


void SimplePeerListManager::UpdateDomainNameForIp(std::string ip, std::string domain) {
    for (auto peer : inPeerList) {
        if (peer->ipaddr == ip)
            peer->hostname = domain;
    }
    for (auto peer : outPeerList) {
        if (peer->ipaddr == ip)
            peer->hostname = domain;
    }
    return;
}
