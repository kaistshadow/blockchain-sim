#ifndef POW_LEDGER_MANAGER_H
#define POW_LEDGER_MANAGER_H

#include <string>
#include <vector>
#include <queue>
#include <list>

#include "ListLedgerManager.h"
#include "POWBlock.h"
#include "../utility/UInt256.h"

// POW ledger manager inherits following class
// 1) std::list based ledger management class (ListLedgerManager)

class POWLedgerManager : public ListLedgerManager<POWBlock> {
 private:
    /* std::list<POWBlock> pow_ledger; */
    
 public:
    POWLedgerManager() {};
    
    bool UpdateLedgerAsLongestChain(POWBlocks *blks);

    /* /\** */
    /*  * Load Ledger from file. */
    /*  * Save Ledger into file. */
    /*  *\/ */
    /* void LoadLedgerFromFile() override; */
    /* void SaveLedgerToFile() override; */


    // all following functions are inherited from ListLedgerManager<POWBlock>
    /* POWBlock *GetLastBlock(); */
    /* unsigned long GetNextBlockIdx(); */
    /* void AppendBlock(POWBlock blk) { list_ledger.push_back(blk); } */
    /* void SetLedger(std::list<POWBlock> blocks) { pow_ledger = blocks; } */
    /* std::list<POWBlock> &GetLedger() { return pow_ledger; } */
    /* void ReplaceLedger(std::list<POWBlock>::iterator pos, std::vector<POWBlock>::iterator start, std::vector<POWBlock>::iterator end); */

};




#endif // POWLEDGERMANAGER_H
