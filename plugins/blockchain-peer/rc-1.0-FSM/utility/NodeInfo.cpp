#include "NodeInfo.h"
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <iostream>

NodeInfo* NodeInfo::instance = 0;

NodeInfo* NodeInfo::GetInstance() {
    if (instance == 0) {
        instance = new NodeInfo();
    }
    return instance;
}

int NodeInfo::GetHostNumber() {
    if (myNodeId.find("bleep") == 0) {
        std::cout << "gethostnumber, nodeid find bleep" << "\n";
        return std::stoi(myNodeId.substr(5, myNodeId.size()));
    }
    else if (myIp != "") {
        unsigned int ips[4];
        sscanf(myIp.c_str(), "%u.%u.%u.%u", &ips[3], &ips[2], &ips[1], &ips[0]);
        unsigned int val = ips[0] + ips[1]*256 + ips[2]*256*256 + ips[3]*256*256*256;
        return val;
    } else
        return -1;
}


// This function is adhoc. It works well on shadow.
// But if you want to use this function in real node, you should be careful.
void NodeInfo::SetHostIP() {
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    char addressBuffer[INET_ADDRSTRLEN];
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;

            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            std::cout << ifa->ifa_name << " IP Address " << addressBuffer << "\n";
        }    
    }
    myIp = std::string(addressBuffer);
    std::cout << "SetHostIP : " << myIp << "\n";
}
