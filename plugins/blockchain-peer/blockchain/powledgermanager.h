#ifndef POWLEDGERMANAGER_H
#define POWLEDGERMANAGER_H

#include <string>
#include <vector>
#include <queue>
#include <list>

#include "listledgermanager.h"
#include "powblock.h"

// POW ledger manager inherits following class
// 1) std::list based ledger management class (ListLedgerManager)

class POWLedgerManager : public ListLedgerManager<POWBlock> {
 private:
    /* POWLedgerManager(std::string filename) : LedgerFileManager(filename) { } // singleton pattern */
    POWLedgerManager(std::string filename) : ListLedgerManager<POWBlock>(filename) { } // singleton pattern
    static POWLedgerManager* instance; // singleton pattern

    /* std::list<POWBlock> pow_ledger; */
    
 public:
    static POWLedgerManager* GetInstance(); // singleton pattern
    static void SetInstance(std::string filename); // singleton pattern

    
    void UpdateLedgerAsLongestChain(POWBlocks *blks);

    utility::UINT256_t CalculateCurrentDifficulty();

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
