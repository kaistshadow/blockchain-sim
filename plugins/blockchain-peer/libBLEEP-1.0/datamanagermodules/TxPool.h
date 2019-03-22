#ifndef TXPOOL_H
#define TXPOOL_H

#include <list>
#include <algorithm>
#include <memory>
#include <boost/shared_ptr.hpp>

#include "../datamodules/Transaction.h"

namespace libBLEEP {

    class TxPool {
    private:
        std::list<boost::shared_ptr<Transaction> > items;
    
    public:
        TxPool() {}
        int GetPendingTxNum() { return items.size(); }

        std::list<boost::shared_ptr<Transaction> > GetTxs(int num);
        void RemoveTxs(const std::list<boost::shared_ptr<Transaction> >& txs);
        void AddTx(boost::shared_ptr<Transaction> tx);
        void AddTxs(std::list<boost::shared_ptr<Transaction> > txs);



        // functions for argument passing between states
    private:
        std::shared_ptr<Transaction> _newly_received_tx;

    public:
        void SetNewlyReceivedTx(std::shared_ptr<Transaction> tx) { _newly_received_tx = tx; }
        void ClearNewlyReceivedTx() { _newly_received_tx = nullptr; }
        
    };

}

#endif 
