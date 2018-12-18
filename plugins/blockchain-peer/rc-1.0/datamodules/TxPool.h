#ifndef TXPOOL_H
#define TXPOOL_H

#include "Transaction.h"
#include <list>
#include <algorithm>

class TxPool {
 private:
    std::list<Transaction> items;

 public:
    TxPool() {}
    
    int GetPendingTxNum() { return items.size(); }

    std::list<Transaction> GetTxs(int num);
    void RemoveTxs(std::list<Transaction> txs);
    void AddTx(Transaction *tx);
    void AddTxs(std::list<Transaction> txs);
        
};

#endif 
