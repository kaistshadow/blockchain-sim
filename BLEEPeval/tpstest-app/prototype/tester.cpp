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
    exported_main(); // initialize bitcoin

  TxBox<TPSPolicy, BitcoinNodePrimitives,BitcoinTxgen> tpsBox;
  tpsBox.setTarget("1.0.0.1", 18333);
    if (!tpsBox.setupNetwork()) {
        std::cout << "setup network failed" << "\n";
        return -1;
    }
  tpsBox.startNetwork();
    std::cout << "connection finished" << "\n";

    return 0;
}
