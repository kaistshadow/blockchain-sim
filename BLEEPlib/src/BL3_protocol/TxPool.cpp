// "Copyright [2021] <kaistshadow>"

#include "TxPool.h"
#include <iostream>

using namespace libBLEEP_BL;

std::list<std::shared_ptr<SimpleTransaction> > TxPool::GetTxs(int num) {
    if (GetPendingTxNum() < num) {
        std::cout << "Warning : Not enough pending transaction pool. Too many requests" << "\n";
        return {};
    }

    std::list<std::shared_ptr<SimpleTransaction> > txs;
    auto it = items.begin();
    while (num-- > 0) {
        txs.push_back(it->second);
        it++;
        // txs.push_back(items.front());
        // items.pop_front();
    }
    return txs;
}

void TxPool::RemoveTxs(const std::list<SimpleTransactionId>& txIds) {
    for (auto& txId : txIds) {
        items.erase(txId);
    }

    return;
}

void TxPool::AddTxs(std::list<std::shared_ptr<SimpleTransaction> > txs) {
    for (auto tx : txs)
        items.insert({tx->GetId(), tx});
}

void TxPool::AddTx(std::shared_ptr<SimpleTransaction> tx) {
    items.insert({tx->GetId(), tx});
    PrintPool();
    return;
}

bool TxPool::ContainTx(SimpleTransactionId tid) {
    return (items.count(tid) > 0);
}

std::shared_ptr<SimpleTransaction> TxPool::GetTx(SimpleTransactionId id) {
    auto it = items.find(id);
    if (it != items.end())
        return it->second;
    else
        return nullptr;
}

void TxPool::PrintPool() {
    std::cout << "======== TXPOOL ========" << "\n";
    for (auto item : items) {
        std::cout << *(item.second) << "\n";
    }
    std::cout << "========================" << "\n";
}
