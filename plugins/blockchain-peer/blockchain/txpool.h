#ifndef TXPOOL_H
#define TXPOOL_H

#include <vector>
#include <queue> 

#include "transaction.h"

class TxPool {
 private:
    TxPool() {}; // singleton pattern
    static TxPool* instance; // singleton pattern

    std::queue<Transaction> msgQueue;

 public:
    static TxPool* GetInstance(); // sigleton pattern

    std::vector<Transaction> items;

    void PushTxToQueue(Transaction tx) { msgQueue.push(tx); }
    void ProcessQueue();
};


#endif
