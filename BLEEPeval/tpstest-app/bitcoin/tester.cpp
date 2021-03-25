//
// Created by ilios on 21. 2. 23..
//

#include <TxBox.h>

#include "BitcoinTxGen.h"

#include <bitcoind.h>

#include <bitcoind.h>
#include <iostream>
#include <unistd.h>

using namespace tpstest;

void _debug_hex_print(void* a, size_t streamsize, int endianess_swap) {
    unsigned char* stream = (unsigned char*)a;
    int cnt = 0;
    if (endianess_swap) {
        for(size_t i=streamsize; i>0; i--) {
            printf("%02x", stream[i-1]);
            if (cnt % 32 == 31) {
                printf("\n");
            } else if (cnt % 8 == 7) {
                printf("   ");
            } else {
                printf(" ");
            }
            cnt++;
        }
    } else {
        for (size_t i=0; i<streamsize; i++) {
            printf("%02x", stream[i]);
            if (cnt % 32 == 31) {
                printf("\n");
            } else if (cnt % 8 == 7) {
                printf("   ");
            } else {
                printf(" ");
            }
            cnt++;
        }
    }
}

#define _TEST_TX_PER_PERIOD  1000
#define _TEST_TIME_PER_PERIOD   1
int main(int argc, char* argv[]) {
    exported_main(); // initialize bitcoin
    ECC_Start();
    ECCVerifyHandle* ecc = new ECCVerifyHandle();

//    BroadcastMessenger<BitcoinMessenger> broadcastMessenger;
//    broadcastMessenger.connect(xxx.xxx.xxx.xxx, xxx);
//    broadcastMessenger.connect(xxx.xxx.xxx.xxx, xxx);
//    broadcastMessenger.connect(xxx.xxx.xxx.xxx, xxx);
//    broadcastMessenger.connect(xxx.xxx.xxx.xxx, xxx);
//    broadcastMessenger.connect(xxx.xxx.xxx.xxx, xxx);

    TxBox<BitcoinTxgen, TPSPolicy ,BitcoinPrimitives> txBox;
//    Messager<BitcoinNodePrimitives> msgr("1.0.0.1", 8333);
    txBox.bootstrap("initial_tx_data.txt", "private_key.txt");

    std::string hexTx = txBox.getNextTxStream();
    std::cout<<"Created Tx: "<<hexTx<<"\n";
    hexTx = txBox.getNextTxStream();
    std::cout<<"Created Tx: "<<hexTx<<"\n";
    hexTx = txBox.getNextTxStream();
    std::cout<<"Created Tx: "<<hexTx<<"\n";
    hexTx = txBox.getNextTxStream();
    std::cout<<"Created Tx: "<<hexTx<<"\n";
    hexTx = txBox.getNextTxStream();
    std::cout<<"Created Tx: "<<hexTx<<"\n";

//    while (true) {
//        for(int i=0; i<_TEST_TX_PER_PERIOD; i++) {
//            std::string hexTx = txBox.getNextTxStream();
//            std::cout<<"Created Tx: "<<hexTx<<"\n";
////            msgr.SendMsg(hexTx);
////            broadcastMessenger.send(txdatastream, streamsize);
//        }
//        sleep(_TEST_TIME_PER_PERIOD);
//    }

    delete ecc;
    return 0;
}
