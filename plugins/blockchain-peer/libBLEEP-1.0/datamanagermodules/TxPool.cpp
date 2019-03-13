#include "TxPool.h"

using namespace libBLEEP;

std::list<std::shared_ptr<Transaction> > libBLEEP::TxPool::GetTxs(int num) {
    if (GetPendingTxNum() < num) {
        std::cout << "Warning : Not enough pending transaction pool. Too many requests" << "\n";
        return {};
    }

    std::list<std::shared_ptr<Transaction> > txs;
    std::list<std::shared_ptr<Transaction> >::iterator it = items.begin();
    while (num-- > 0) {
        txs.push_back(*it);
        it++;
        // txs.push_back(items.front());
        // items.pop_front();
    }
    return txs;
}

void libBLEEP::TxPool::RemoveTxs(const std::list<std::shared_ptr<Transaction> >& txs) {
    std::list<std::shared_ptr<Transaction> >::iterator i = items.begin();

    while (i != items.end()) {
        // bool found = (std::find(txs.begin(), txs.end(), *i) != txs.end());
        // if (found)
        //     items.erase(i++);

        std::shared_ptr<Transaction> pool_tx = (*i);
        auto it = std::find_if(txs.begin(), txs.end(), [pool_tx](std::shared_ptr<Transaction> const& t){
                    return *pool_tx == *t;
                });
        if (it != txs.end()) {
            items.erase(i++);
        }
        else
            ++i;
    }
}

void libBLEEP::TxPool::AddTxs(std::list<std::shared_ptr<Transaction> > txs) {
    for (auto tx : txs)
        items.push_back(tx);
}

void libBLEEP::TxPool::AddTx(std::shared_ptr<Transaction> tx) {
    items.push_back(tx);
    return;
}
