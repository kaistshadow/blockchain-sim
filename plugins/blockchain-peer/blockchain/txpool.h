#ifndef TXPOOL_H
#define TXPOOL_H

#include <list>
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

    std::list<Transaction> items;

    void PushTxToQueue(Transaction tx) { msgQueue.push(tx); }
    void ProcessQueue();

    int GetPendingTxNum() { return items.size(); }
    std::list<Transaction> GetTxs(int num);
    void RemoveTxs(std::list<Transaction> txs);
    void AddTxs(std::list<Transaction> txs);
};


#endif
