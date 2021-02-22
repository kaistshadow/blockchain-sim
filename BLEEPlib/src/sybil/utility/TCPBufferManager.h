//
// Created by ilios on 21. 2. 15..
//

#ifndef TCPMANAGER_H
#define TCPMANAGER_H

#include <string>
#include <unistd.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h> /* Added for the nonblocking socket */
#include <iostream>

namespace libBLEEP_sybil {
    class TCPBufferManager {
    public:
        TCPBufferManager() {}

        void PortConnect(std::string ip, int port, void (*callbackAfterConnect)());

    private:


    };
}

#endif //BLEEP_TCPMANAGER_H
