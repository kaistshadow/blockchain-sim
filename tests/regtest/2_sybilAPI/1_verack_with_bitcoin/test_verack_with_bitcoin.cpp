//
// Created by ilios on 21. 3. 4..
//

#include <bitcoind.h>


#include "BitcoinNodePrimitives.h"

#include <ipdb/SimpleIPDatabase.h>
#include <node/ShadowActiveNode.h>


using namespace sybiltest;

int main() {
    exported_main(); // initialize bitcoin : This is necessary for using Bitcoin's benign/shadow node

    // Initialize basic class for logging & IP management
    AttackStat attackStat; // kind of logger
    SimpleIPDatabase dummyIPDatabase;

    // Initialize single benign node
    ShadowActiveNode<BitcoinNodePrimitives> activeBitcoinNode(&attackStat, &dummyIPDatabase, "2.0.0.1");
    activeBitcoinNode.tryConnectToTarget("1.0.0.1", 8333, 5);

    // Start attack
    struct ev_loop *libev_loop = EV_DEFAULT;
    ev_run(libev_loop, 0);
    std::cout << "all watchers are removed" << "\n";

}