#include "HandleTransaction_BasicModel.h"

#include <iostream>

void HandleTransaction_BasicModel::HandleArrivedTx(Transaction *tx) {
    std::cout << "inject a arrived transaction to TXPOOL" << "\n";
    txpool.AddTx(tx);
}
