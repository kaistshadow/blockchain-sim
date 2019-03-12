#ifndef LEDGER_MANAGER_H
#define LEDGER_MANAGER_H

#include <string>
#include <vector>
#include <queue>
#include <list>

#include "ListLedgerManager.h"
#include "../datamodules/Block.h"
#include "../utility/UInt256.h"

// ledger manager inherits following class
// 1) std::list based ledger management class (ListLedgerManager)

class LedgerManager : public ListLedgerManager<Block> {
 private:
    
 public:
    LedgerManager() {};
    
};




#endif
