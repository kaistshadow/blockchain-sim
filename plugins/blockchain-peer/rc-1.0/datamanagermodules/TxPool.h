#ifndef TXPOOL_H
#define TXPOOL_H

#include <list>
#include <algorithm>
#include <boost/shared_ptr.hpp>

#include "../datamodules/Transaction.h"

class TxPool {
 private:
    TxPool() {}; // singleton pattern
    static TxPool* instance; // singleton pattern

    std::list<boost::shared_ptr<Transaction> > items;

 public:
    static TxPool* GetInstance(); // sigleton pattern
    
    int GetPendingTxNum() { return items.size(); }

    std::list<boost::shared_ptr<Transaction> > GetTxs(int num);
    void RemoveTxs(const std::list<boost::shared_ptr<Transaction> >& txs);
    void AddTx(boost::shared_ptr<Transaction> tx);
    void AddTxs(std::list<boost::shared_ptr<Transaction> > txs);
        
};

#endif 
