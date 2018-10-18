#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/list.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "powledgermanager.h"
#include "../blockchain/txpool.h"

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

void POWLedgerManager::LoadLedgerFromFile() {
    boost::filesystem::path myFile = boost::filesystem::current_path() / ledger_filename;

    if (boost::filesystem::exists(myFile)) {
        boost::filesystem::ifstream ifs(myFile/*.native()*/);
        boost::archive::text_iarchive ta(ifs);

        ta >> list_ledger; // foo is empty until now, it's fed by myFile

        std::cout << "Read ledger from " << myFile << "\n";
    }
}

void POWLedgerManager::SaveLedgerToFile() {
    boost::filesystem::path myFile = boost::filesystem::current_path() / ledger_filename;
    boost::filesystem::ofstream ofs(myFile);
    boost::archive::text_oarchive ta(ofs);

    ta << list_ledger; 
    std::cout << "Wrote ledger to " << myFile << "\n";
    return;
}

