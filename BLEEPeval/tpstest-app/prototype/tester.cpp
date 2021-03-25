//
// Created by ilios on 21. 2. 23..
//

#include <bitcoind.h>
#include <TxBox.h>
#include <policy/TPSPolicy.h>

#include "BitcoinNodePrimitives.h"
#include "BitcoinTxGen.h"

using namespace tpstest;
int main(int argc, char* argv[]) {
  int targetNum = atoi(argv[1]);
  exported_main(); // initialize bitcoin

  TxBox<TPSPolicy, BitcoinNodePrimitives,BitcoinTxgen> tpsBox;
  tpsBox.addNode("99.99.0.1",18333);

  for(int i=0; i<targetNum; i++){
    std::string address;
    int port = 18333;
    sprintf(&address[0], "1.%d.0.1", i);
    tpsBox.addTarget(address.c_str(), port);
  }

    if (!tpsBox.setupNetwork()) {
        std::cout << "setup network failed" << "\n";
        return -1;
    }
  tpsBox.startNetwork();
    std::cout << "connection finished" << "\n";

    return 0;
}
