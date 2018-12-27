#ifndef LIST_LEDGER_MANAGER_H
#define LIST_LEDGER_MANAGER_H

#include <string>
#include <vector>
#include <queue>
#include <list>
#include <type_traits>

#include "../datamodules/Block.h"
#include "TxPool.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

template <typename T> 
class ListLedgerManager {
    static_assert(std::is_base_of<Block, T>::value, "T must inherit from Block");
 protected:
    std::list<T> list_ledger;
    std::string ledger_filename;
    
 public:
    ListLedgerManager() {};
    ListLedgerManager(std::string filename) { ledger_filename = filename; }

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

    // Load ledger from file
    void LoadLedgerFromFile();
    void SaveLedgerToFile();

};

#include "ListLedgerManager_impl.h"

#endif // LEDGERMANAGER_H
