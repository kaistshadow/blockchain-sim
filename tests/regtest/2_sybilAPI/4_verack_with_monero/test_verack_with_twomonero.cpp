//
// Created by ilios on 21. 4. 5..
//

#include "MoneroNodePrimitives.h"

#include <ipdb/SimpleIPDatabase.h>
#include <node/ShadowActiveNode.h>


using namespace sybiltest;

int main() {
    // initialize monero : This is necessary for using Monero's benign/shadow node
    std::cout << "verack tester for monero started" << "\n";

    // Initialize basic class for logging & IP management
    AttackStat attackStat; // kind of logger
    SimpleIPDatabase dummyIPDatabase;

    // Initialize first benign node
    ShadowActiveNode<MoneroNodePrimitives> activeMoneroNode(&attackStat, &dummyIPDatabase, "2.0.0.1");
    activeMoneroNode.tryConnectToTarget("1.0.0.1", 28080, 5);

    // Initialize second benign node
    ShadowActiveNode<MoneroNodePrimitives> activeMoneroNode2(&attackStat, &dummyIPDatabase, "3.0.0.1");
    activeMoneroNode2.tryConnectToTarget("1.0.0.1", 28080, 6);

    // Start attack
    struct ev_loop *libev_loop = EV_DEFAULT;
    ev_run(libev_loop, 0);
    std::cout << "all watchers are removed" << "\n";

}