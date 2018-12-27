#ifndef TXPOOL_H
#define TXPOOL_H

#include "../datamodules/Transaction.h"
#include <list>
#include <algorithm>

class TxPool {
 private:
    TxPool() {}; // singleton pattern
    static TxPool* instance; // singleton pattern

    std::list<Transaction> items;

 public:
    static TxPool* GetInstance(); // sigleton pattern
    
    int GetPendingTxNum() { return items.size(); }

    std::list<Transaction> GetTxs(int num);
    void RemoveTxs(const std::list<Transaction>& txs);
    void AddTx(Transaction *tx);
    void AddTxs(std::list<Transaction> txs);
        
};

#endif 
