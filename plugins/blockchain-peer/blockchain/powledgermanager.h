#ifndef POWLEDGERMANAGER_H
#define POWLEDGERMANAGER_H

#include <string>
#include <vector>
#include <queue>
#include <list>

#include "ledgermanager.h"
#include "powblock.h"

class POWLedgerManager {
 private:
    POWLedgerManager(std::string filename); // singleton pattern
    static POWLedgerManager* instance; // singleton pattern

    /* std::string ledger_filename; */
    /* std::list<Block> ledger; */
    std::string ledger_filename;
    std::list<POWBlock> pow_ledger;
    
 public:
    static POWLedgerManager* GetInstance(); // singleton pattern
    static void SetInstance(std::string filename); // singleton pattern

    
    POWBlock *GetLastBlock();

    unsigned long GetNextBlockIdx();
    void AppendBlock(POWBlock blk) { pow_ledger.push_back(blk); }

    /* // init ledger from file or from network */
    void InitLedger();

    /**
     * Load Ledger from file.
     * Save Ledger into file.
     */
    void LoadLedgerFromFile();
    void SaveLedgerToFile();

    /**
     * Set and Get a server socket
     */
    void SetLedger(std::list<POWBlock> blocks) { pow_ledger = blocks; }
    std::list<POWBlock> GetLedger() { return pow_ledger; }

};




#endif // POWLEDGERMANAGER_H
