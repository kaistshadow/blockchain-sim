#ifndef TXPOOL_H
#define TXPOOL_H

#include <list>
#include <map>
#include <algorithm>
#include <memory>

//#include <boost/shared_ptr.hpp>

#include "Transaction.h"

namespace libBLEEP_BL {

    // TODO : Convert TxPool into generic class using template
    //        Currently, it only support SimpleTransaction
    class TxPool {
    private:
        std::map<SimpleTransactionId, std::shared_ptr<SimpleTransaction>> items;
    
    public:
        TxPool() {}
        int GetPendingTxNum() { return items.size(); }

        std::list<std::shared_ptr<SimpleTransaction> > GetTxs(int num);
        void RemoveTxs(const std::list<SimpleTransactionId >& txIds);
        void AddTx(std::shared_ptr<SimpleTransaction> tx);
        void AddTxs(std::list<std::shared_ptr<SimpleTransaction> > txs);

        bool ContainTx(SimpleTransactionId tid);
        std::shared_ptr<SimpleTransaction> GetTx(SimpleTransactionId id);

        void PrintPool();

    };

}

#endif 
