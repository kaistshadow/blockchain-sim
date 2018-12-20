#include "../datamodules/Transaction.h"
#include "HandleTransaction_BasicModel.h"

#include <iostream>
#include <stdlib.h>

void HandleTransaction_BasicModel::PrintTransactions() {
    int i = 1;
    for (Transaction t : txpool.GetTxs(txpool.GetPendingTxNum())) {
        std::cout << t << "\n";
        // std::cout << "Tx " << i << ":" << t << "\n";
        i++;
    }
}

Transaction HandleTransaction_BasicModel::MakeRandomValidTransaction() {
    // currently, it does not consider the validity of the transaction.
    
    srand (time(NULL));
    unsigned int random_num = rand();
    int from = random_num % 99;
    int to = random_num % 101;
    double value = random_num % 1000;
    return Transaction(from, to, value);
}

void HandleTransaction_BasicModel::HandleArrivedTx(Transaction *tx) {
    std::cout << "inject a arrived transaction to TXPOOL" << "\n";
    txpool.AddTx(tx);
    
    if (txpool.GetPendingTxNum() == 100) {
        PrintTransactions();
    }
}
