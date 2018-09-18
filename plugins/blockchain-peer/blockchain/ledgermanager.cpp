#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/list.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "ledgermanager.h"

LedgerManager::LedgerManager(ConsensusDriver& driver, std::string filename)
    : consensusDriver(driver) {
    ledger_filename = filename; 
}

LedgerManager* LedgerManager::instance = 0;

LedgerManager* LedgerManager::GetInstance() {
    if (instance == 0) {
        std::cout << "GetInstance before setInstnace" << "\n";
        exit(-1);
    }
    return instance;
}
void LedgerManager::SetInstance(ConsensusDriver& driver, std::string filename) {
    if (instance == 0) {
        instance = new LedgerManager(driver, "blk.dat");
    }
}




void LedgerManager::LoadLedgerFromFile() {
    boost::filesystem::path myFile = boost::filesystem::current_path() / ledger_filename;

    if (boost::filesystem::exists(myFile)) {
        boost::filesystem::ifstream ifs(myFile/*.native()*/);
        boost::archive::text_iarchive ta(ifs);

        ta >> ledger; // foo is empty until now, it's fed by myFile

        std::cout << "Read ledger from " << myFile << "\n";
    }
}

void LedgerManager::SaveLedgerToFile() {
    boost::filesystem::path myFile = boost::filesystem::current_path() / ledger_filename;
    boost::filesystem::ofstream ofs(myFile);
    boost::archive::text_oarchive ta(ofs);

    ta << ledger; 
    std::cout << "Wrote ledger to " << myFile << "\n";
    return;
}

void LedgerManager::InitLedger() {
    LoadLedgerFromFile();
}

void LedgerManager::Loop() {
    // This loop is called periodically (by node's main loop)
    // In this loop, it controls consensus among p2p nodes.
    // Using this loop, the channel between ledgermanager and consensus class is implemented.
    // For generalize such interface, we leverage ConsensusDriver interface.
    // Any consensus protocol should implement its own driver. (ex. StellarConsensusDriver for SCP.)


    // Check if the consensus protocol has any new consensus results.
    // If there exists new consensus result, update ledger.
    if (consensusDriver.HasNewConsensus()) {
        consensusDriver.UpdateLedger();
    } 

    // Check if the consensus protocol is ready to trigger next consensus slot.
    // If it is ready, trigger the next consensus.
    // For example, if it uses stellar consensus protocol,
    // it initiates SCP's nomination protocol periodically.
    if (consensusDriver.PrepareNextConsensus()) 
        consensusDriver.TriggerNextConsensus();
}
