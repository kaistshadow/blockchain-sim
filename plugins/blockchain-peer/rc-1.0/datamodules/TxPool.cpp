#include "TxPool.h"

TxPool* TxPool::instance = 0;

TxPool* TxPool::GetInstance() {
    if (instance == 0) {
        instance = new TxPool();
    }
    return instance;
}

std::list<Transaction> TxPool::GetTxs(int num) {
    if (GetPendingTxNum() < num) {
        std::cout << "Warning : Not enough pending transaction pool. Too many requests" << "\n";
        return {};
    }

    std::list<Transaction> txs;
    std::list<Transaction>::iterator it = items.begin();
    while (num-- > 0) {
        txs.push_back(*it);
        it++;
        // txs.push_back(items.front());
        // items.pop_front();
    }
    return txs;
}

void TxPool::RemoveTxs(const std::list<Transaction>& txs) {
    std::list<Transaction>::iterator i = items.begin();
    while (i != items.end()) {
        bool found = (std::find(txs.begin(), txs.end(), *i) != txs.end());
        if (found)
            items.erase(i++);
        else
            ++i;
    }
}

void TxPool::AddTxs(std::list<Transaction> txs) {
    for (auto tx : txs)
        items.push_back(tx);
}

void TxPool::AddTx(Transaction *tx) {
    items.push_back(*tx);
    return;
}
