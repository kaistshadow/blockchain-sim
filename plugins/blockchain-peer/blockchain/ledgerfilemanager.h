#ifndef LEDGERFILEMANAGER_H
#define LEDGERFILEMANAGER_H


#include <string>
#include <vector>
#include <queue>
#include <list>

class LedgerFileManager {
 protected:
    std::string ledger_filename;
    
 public:
    LedgerFileManager(std::string filename) { ledger_filename = filename; }

    /**
     * Load Ledger from file.
     * Save Ledger into file.
     */
    virtual void LoadLedgerFromFile() = 0;
    virtual void SaveLedgerToFile() = 0;
};




#endif // LEDGERFILEMANAGER_H
