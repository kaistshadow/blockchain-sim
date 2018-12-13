#include <iostream>
#include <memory>
#include "Configuration.h"
#include "event/GlobalEvent.h"

std::shared_ptr<HandleNetwork> handleNetwork;
bool amIProxyNode = false;

int main(int argc, char *argv[]) {

    if (argc == 2) {
        if (std::string(argv[1]) == "proxy")
            amIProxyNode = true;
    }
    std::cout << "Testing node up" << "\n";

    // Allocate handlers for each state
    handleNetwork = HandleNetwork::create(HANDLE_NETWORK_TEST);


    // Allocate event loop handler
    GlobalEvent::loop = ev_default_loop(0);

    // Join the P2P network
    int result = handleNetwork->JoinNetwork();
    if (result != 0) {
        std::cout << "Failed to join Network. Exit" << "\n";
        exit(-1);
    }

    // Start event loop, i.e., start the Blockchain State Machine by jumping into idle state.
    std::cout << "before ev_loop" << "\n";
    ev_loop(GlobalEvent::loop, 0);
}

