// "Copyright [2021] <kaistshadow>"

//
// Created by ilios on 21. 2. 23..
//

#include <bitcoind.h>
#include <AttackBox.h>
#include <policy/ErebusAttackPolicy.h>
#include <ipdb/BitcoinIPDatabase.h>

#include "BitcoinNodePrimitives.h"
#include "BitcoinNodeParams.h"

using namespace sybiltest;

int main() {
    exported_main();  // initialize bitcoin

    AttackBox<ErebusAttackPolicy, BitcoinNodePrimitives, BitcoinNodeParams, BitcoinIPDatabase> attackBox;
    attackBox.setTarget("1.0.0.1", 8333);
    if (!attackBox.setupAttack()) {
        std::cout << "setup attack failed" << "\n";
        return -1;
    }
    attackBox.startAttack();
    std::cout << "attack finished" << "\n";

    return 0;
}
