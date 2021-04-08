//
// Created by ilios on 21. 3. 5..
//

#include <bitcoind.h>

#include <AttackBox.h>
#include <policy/ErebusAttackPolicy.h>
#include <ipdb/BitcoinIPDatabase.h>
#include "BitcoinNodePrimitives.h"

using namespace sybiltest;

struct MiniBitcoinNodeParams {
    //============================== parameters related to EREBUS attack test ======================================
    static constexpr int targetPort = 8333;
    static constexpr int targetOutgoingConnNum = 2;
    static constexpr int reachableIPNum = 10;
    static constexpr int unreachableIPNum = 0;
    static constexpr int shadowIPNum = 100;
    static constexpr int addrInjectionStartTime = 50;  //
    static constexpr int addrInjectionDelay = 50;
    static constexpr double addrInjectionIPPerSec = 2;
    static constexpr double addrInjectionShadowRate = 0.9;
    static constexpr std::chrono::system_clock::duration preparePhaseTimeLength = std::chrono::minutes(2);
};


int main() {
    exported_main(); // initialize bitcoin : This is necessary for using Bitcoin's benign/shadow node

    AttackBox<ErebusAttackPolicy, BitcoinNodePrimitives, MiniBitcoinNodeParams, BitcoinIPDatabase> attackBox;
    attackBox.setTarget("1.0.0.1", 8333);

    if (!attackBox.setupAttack()) {
        std::cout << "setup attack failed" << "\n";
        return -1;
    }

    // Modify churnout timer
    auto it = attackBox.GetBenignNodes().begin();
    auto &benign1 = *it;
    auto &benign2 = *(++it);
    benign1.SetChurnOutTimer(60);
    benign2.SetChurnOutTimer(120);

    attackBox.startAttack();
    std::cout << "attack finished" << "\n";

    return 0;
}