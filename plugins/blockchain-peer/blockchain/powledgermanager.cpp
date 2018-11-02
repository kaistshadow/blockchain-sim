#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/list.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "powledgermanager.h"
#include "../blockchain/txpool.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

POWLedgerManager* POWLedgerManager::instance = 0;

POWLedgerManager* POWLedgerManager::GetInstance() {
    if (instance == 0) {
        std::cout << "GetInstance before setInstnace" << "\n";
        exit(-1);
    }
    return instance;
}
void POWLedgerManager::SetInstance(std::string filename) {
    if (instance == 0) {
        instance = new POWLedgerManager("blk.dat");
    }
}

// Check whether the given blocks (blks) is longer chain than currently managed block chain (list_ledger)
// If given blockchain has a block whose index is higher than current block chain, 
// then update the current chain from forked position.
void POWLedgerManager::UpdateLedgerAsLongestChain(POWBlocks* blks) {
    int lastblkidx = list_ledger.size() - 1;
    int given_lastblkidx = blks->back().GetBlockIdx();
    if (given_lastblkidx <= list_ledger.size()-1)
        return;

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
        DumpLedgerToJSONFile("ledger.json");
    }
}

utility::UINT256_t POWLedgerManager::CalculateCurrentDifficulty() {
    unsigned char default_th[32] = {0x00,0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

    if (list_ledger.size() < 2)
        return utility::UINT256_t(default_th, 32);
    if (list_ledger.back().GetDifficulty() == 0)
        return utility::UINT256_t(default_th, 32);

    std::list<POWBlock>::reverse_iterator rit = list_ledger.rbegin();
    double last_timestamp = (*rit).GetTimestamp();
    utility::UINT256_t last_difficulty = (*rit).GetDifficulty();
    
    int i = 0;
    std::vector<double> time_diffs;

    rit++;
    while (rit != list_ledger.rend() && i < 100) {
        double past_timestamp = (*rit).GetTimestamp();
        double time_diff = last_timestamp - past_timestamp;
        time_diffs.push_back(time_diff);
        last_timestamp = past_timestamp;

        rit++;
        i++;
    }

    
    int short_diff_num = 0;
    int proper_diff_num = 0;
    int long_diff_num = 0;
    for (auto time_diff : time_diffs) {
        if (time_diff < 1)
            short_diff_num++;
        else if (2 <= time_diff && time_diff <= 50)
            proper_diff_num++;
        else if (50 < time_diff)
            long_diff_num++;
    }

    utility::UINT256_t new_difficulty;
    if (short_diff_num > proper_diff_num + long_diff_num)
        new_difficulty = last_difficulty >> 1;
    else if (proper_diff_num > short_diff_num + long_diff_num)
        new_difficulty = last_difficulty;
    else if (long_diff_num > short_diff_num + proper_diff_num)
        new_difficulty = (last_difficulty << 1) | 0x1;
    else
        new_difficulty = last_difficulty;
    
    // if (0.5 <= time_diff && time_diff < 10)
    //     new_difficulty = last_difficulty;
    // else if (time_diff < 0.5)
    //     new_difficulty = last_difficulty >> 1;
    // else if (time_diff > 10)
    //     new_difficulty = (last_difficulty << 1) | 0x1;
        
    // std::cout  << "last_timestamp:"<< last_timestamp <<",past_timestamp:" <<past_timestamp <<",time_diff=" <<time_diff << ",last_difficulty="<<last_difficulty << ",new_diffi=" << new_difficulty << "\n";

    return new_difficulty;
            
}
