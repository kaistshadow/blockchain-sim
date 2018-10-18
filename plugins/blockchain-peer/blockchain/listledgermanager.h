#ifndef LISTLEDGERMANAGER_H
#define LISTLEDGERMANAGER_H

#include <string>
#include <vector>
#include <queue>
#include <list>

#include "txpool.h"

template <typename T> 
class ListLedgerManager {
 protected:
    std::list<T> list_ledger;
    
 public:
    /**
     * Set and Get a ledger
     */
    void SetLedger(std::list<T> blocks) { list_ledger = blocks; }
    std::list<T> &GetLedger() { return list_ledger; }

    T* GetLastBlock();
    unsigned long GetNextBlockIdx() { return list_ledger.size(); }
    void AppendBlock(T blk) { list_ledger.push_back(blk); }

    // Replace a ledger
    void ReplaceLedger(typename std::list<T>::iterator pos, typename std::vector<T>::iterator start, typename std::vector<T>::iterator end); 

};

#include "listledgermanager.tpp"

#endif // LEDGERMANAGER_H
