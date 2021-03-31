//
// Created by ilios on 21. 2. 23..
//

#include <bitcoind.h>
#include <TxBox.h>
#include <policy/MonitoringPolicy.h>

#include "BitcoinNodePrimitives.h"

using namespace tpstest;
int main(int argc, char* argv[]) {
    std::string ipaddr = argv[1];
    int targetNum = atoi(argv[2]);
  exported_main(); // initialize bitcoin
  ECC_Start();
  ECCVerifyHandle* ecc = new ECCVerifyHandle();

  TxBox<MonitoringPolicy, BitcoinNodePrimitives> MonitorBox;
  MonitorBox.addNode(ipaddr,18333);

  for(int i=0; i<targetNum; i++){
    std::string address;
    int port = 18333;
    sprintf(&address[0], "1.%d.0.1", i);
    MonitorBox.addTarget(address.c_str(), port);
  }

  if (!MonitorBox.setupNetwork()) {
    std::cout << "setup network failed" << "\n";
    return -1;
  }
  MonitorBox.startNetwork();
  std::cout << "connection finished" << "\n";

  return 0;
}