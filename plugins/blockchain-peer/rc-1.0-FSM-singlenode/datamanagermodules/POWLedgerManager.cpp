#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/list.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "POWLedgerManager.h"


// Check whether the given blocks (blks) is longer chain than currently managed block chain (list_ledger)
// If given blockchain has a block whose index is higher than current block chain, 
// then update the current chain from forked position.
bool POWLedgerManager::UpdateLedgerAsLongestChain(POWBlocks* blks) {
    int given_lastblkidx = blks->back().GetBlockIdx();
    if ((size_t)given_lastblkidx <= list_ledger.size()-1)
        return false;

    std::list<POWBlock>::iterator ledger_it = list_ledger.begin();
    std::vector<POWBlock>::iterator received_blks_it = blks->begin();    

    // This while loop is for the situation where the given blks is partial blockchain
    while (ledger_it != list_ledger.end() &&
           ledger_it->GetBlockIdx() != received_blks_it->GetBlockIdx()) {
        ledger_it++;
    }
    // Now blocks pointed by ledger_it and received_blks_it have same index

    while (ledger_it != list_ledger.end() && received_blks_it != blks->end() &&
           ledger_it->GetBlockHash() == received_blks_it->GetBlockHash()) {
        ledger_it++;
        received_blks_it++;
    }
    // Now blocks pointed by ledger_it and received_blks_it have same index but different hash

    if (received_blks_it != blks->end()) {
        ReplaceLedger(ledger_it, received_blks_it, blks->end());
        return true;
    }
    else
        return false;
}

