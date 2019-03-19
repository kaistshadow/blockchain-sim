#ifndef LEDGERMANAGER_H
#define LEDGERMANAGER_H

#include <string>
#include <vector>
#include <queue>
#include <list>

#include "../consensus/consensusdriver.h"
#include "block.h"

class LedgerManager {
 private:
    LedgerManager(std::string filename); // singleton pattern
    /* LedgerManager(ConsensusDriver& driver, std::string filename);  // singleton pattern */
    static LedgerManager* instance; // singleton pattern

    std::string ledger_filename;
    std::list<Block> ledger;
    /* ConsensusDriver& consensusDriver;  // interface for controlling consensus */
    
 public:
    static LedgerManager* GetInstance(); // singleton pattern
    static void SetInstance(std::string filename); // singleton pattern
    /* static void SetInstance(ConsensusDriver& driver, std::string filename); // singleton pattern */

    // main loop procedure for triggering a consensus for new block
    void Loop();

    // init ledger from file or from network
    void InitLedger();

    // allocate consensus driver
    /* void InitConsensusDriver(ConsensusDriver& driver) { consensusDriver = driver; } */

    /**
     * Load Ledger from file.
     * Save Ledger into file.
     */
    void LoadLedgerFromFile();
    void SaveLedgerToFile();

    /**
     * Set and Get a server socket
     */
    void SetLedger(std::list<Block> blocks) { ledger = blocks; }
    std::list<Block> GetLedger() { return ledger; }

};




#endif // LEDGERMANAGER_H
