//
// Created by ilios on 21. 3. 4..
//

#include <bitcoind.h>

#include "ipdb/SimpleIPDatabase.h"
#include "nodePrimitives/BitcoinNodePrimitives.h"
#include "node/BenignNode.h"

using namespace libBLEEP_sybil;

int main() {
    exported_main(); // initialize bitcoin : This is necessary for using Bitcoin's benign/shadow node

    // Initialize basic class for logging & IP management
    AttackStat attackStat; // kind of logger
    SimpleIPDatabase dummyIPDatabase;

    // Initialize single benign node
    BenignNode<BitcoinNodePrimitives> benignNode(&attackStat, &dummyIPDatabase, "2.0.0.1", 8333);
    benignNode.SetTarget("1.0.0.1", 8333);
    benignNode.SetChurnOutTimer(30);

    // Start attack
    struct ev_loop *libev_loop = EV_DEFAULT;
    ev_run(libev_loop, 0);
    std::cout << "all watchers are removed" << "\n";

}