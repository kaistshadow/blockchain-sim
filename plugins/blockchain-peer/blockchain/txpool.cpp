#include <iostream>

#include "txpool.h"

using namespace std;

TxPool* TxPool::instance = 0;

TxPool* TxPool::GetInstance() {
    if (instance == 0) {
        instance = new TxPool();
    }
    return instance;
}

void TxPool::ProcessQueue() {
    while (!msgQueue.empty()) {
        Transaction tx = msgQueue.front();
        items.push_back(tx);
        cout << "Transaction(" << tx << ") is inserted into TxPool" << "\n";
        cout << tx << "\n";
        msgQueue.pop();
    }
}
