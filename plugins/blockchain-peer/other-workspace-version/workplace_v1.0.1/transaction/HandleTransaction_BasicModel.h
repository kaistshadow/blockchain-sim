#ifndef HANDLE_TRANSACTION_BASIC_H
#define HANDLE_TRANSACTION_BASIC_H

#include "HandleTransaction.h"

//
#include "../datamanagermodules/TxPool.h"

class HandleTransaction_BasicModel: public HandleTransaction {
 private:
    void PrintTransactions();

 public:
    HandleTransaction_BasicModel() {}
    virtual ~HandleTransaction_BasicModel() {}

    void HandleArrivedTx(boost::shared_ptr<Transaction> tx);
    boost::shared_ptr<Transaction> MakeRandomValidTransaction();
};


#endif
