#include "TxPool.h"

TxPool* TxPool::instance = 0;

TxPool* TxPool::GetInstance() {
    if (instance == 0) {
        instance = new TxPool();
    }
    return instance;
}

std::list<boost::shared_ptr<Transaction> > TxPool::GetTxs(int num) {
    if (GetPendingTxNum() < num) {
        std::cout << "Warning : Not enough pending transaction pool. Too many requests" << "\n";
        return {};
    }

    std::list<boost::shared_ptr<Transaction> > txs;
    std::list<boost::shared_ptr<Transaction> >::iterator it = items.begin();
    while (num-- > 0) {
        txs.push_back(*it);
        it++;
        // txs.push_back(items.front());
        // items.pop_front();
    }
    return txs;
}

void TxPool::RemoveTxs(const std::list<boost::shared_ptr<Transaction> >& txs) {
    std::list<boost::shared_ptr<Transaction> >::iterator i = items.begin();

    while (i != items.end()) {
        // bool found = (std::find(txs.begin(), txs.end(), *i) != txs.end());
        // if (found)
        //     items.erase(i++);

        boost::shared_ptr<Transaction> pool_tx = (*i);
        auto it = std::find_if(txs.begin(), txs.end(), [pool_tx](boost::shared_ptr<Transaction> const& t){
                    return *pool_tx == *t;
                });
        if (it != txs.end()) {
            items.erase(i++);
        }
        else
            ++i;
    }
}

void TxPool::AddTxs(std::list<boost::shared_ptr<Transaction> > txs) {
    for (auto tx : txs)
        items.push_back(tx);
}

void TxPool::AddTx(boost::shared_ptr<Transaction> tx) {
    items.push_back(tx);
    return;
}
