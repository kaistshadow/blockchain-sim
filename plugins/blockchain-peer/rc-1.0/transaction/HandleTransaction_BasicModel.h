#ifndef HANDLE_TRANSACTION_BASIC_H
#define HANDLE_TRANSACTION_BASIC_H

#include "HandleTransaction.h"
#include "../datamodules/Transaction.h"
#include "../datamodules/TxPool.h"

class HandleTransaction_BasicModel: public HandleTransaction {
 private:
    void PrintTransactions();

 public:
    HandleTransaction_BasicModel() {}
    virtual ~HandleTransaction_BasicModel() {}

    void HandleArrivedTx(Transaction *tx);
    Transaction MakeRandomValidTransaction();
};


#endif
