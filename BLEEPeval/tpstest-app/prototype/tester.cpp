//
// Created by ilios on 21. 2. 23..
//

#include <bitcoind.h>
#include <TxBox.h>
#include <policy/TPSPolicy.h>

#include "BitcoinNodePrimitives.h"
#include "BitcoinTxGen.h"

using namespace tpstest;
int main() {
//    exported_main(); // initialize bitcoin

  TxBox<TPSPolicy, BitcoinNodePrimitives,BitcoinTxgen> tpsBox;
  tpsBox.setTarget("1.0.0.1", 18333);
    if (!tpsBox.setupAttack()) {
        std::cout << "setup attack failed" << "\n";
        return -1;
    }
  tpsBox.startAttack();
    std::cout << "attack finished" << "\n";

    return 0;
}
