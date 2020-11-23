//
// Created by ilios on 20. 10. 19
//

#include <ev++.h>

#include <algorithm>
#include <string_view>
#include <iostream>
#include <deque>

#include "shadow_interface.h"
#include <arpa/inet.h>
#include <fcntl.h> /* Added for the nonblocking socket */
#include <memory>

#include <util/system.h>
#include <bitcoind.h>
#include <net.h>
#include <net_processing.h>

template<typename MSG, void (*ReceiveMSG)(MSG, int)>
struct Node {
    Node() {
        std::cout << "Node generated" << "\n";
    }

    void testRecvMsg(int a) {
        MSG msg;
//        msg.content = "test";
        (*ReceiveMSG)(msg, a);
    }
};

class BitcoinMsg{
public:
    std::string content;
    BitcoinMsg() {content = "initialized";}
};

void BitcoinRecvMsg(BitcoinMsg msg, int a) {
    std::cout << "BitcoinRecvMsg function " << msg.content << "," << a << "\n";
    return;
}

int main(int argc, char *argv[]) {

    Node<BitcoinMsg, BitcoinRecvMsg> n;
    n.testRecvMsg(10);

//    while (true) {
//        std::cout << "before ev_run" << "\n";
//        ev_run (libev_loop, EVRUN_ONCE);
//    }
}
