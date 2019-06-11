template <typename T>
T* ListLedgerManager<T>::GetLastBlock() {
   if (list_ledger.size() == 0) 
       return nullptr;
   else
       return &list_ledger.back();
}


template <typename T>
void ListLedgerManager<T>::AppendBlock(typename std::shared_ptr<T> blk) {
    list_ledger.push_back(*blk);
    /* std::cout << GetGlobalClock() << ":block appended" << "\n"; */
    /* std::cout << *blk << "\n"; */

    auto pow_blk = std::dynamic_pointer_cast<POWBlock>(blk);
    if (pow_blk) {
        // append shadow log
        char buf[256];
        
        std::stringstream str, str2;
        std::ostringstream ss, ss2;
        str << pow_blk->GetBlockHash();
        ss << str.rdbuf();
        std::string blockHash = ss.str();

        str2 << pow_blk->GetPrevBlockHash();
        ss2 << str2.rdbuf();
        std::string prevBlockHash = ss2.str();
        sprintf(buf, "BlockAppend,%lu,%s,%s,%f", 
                pow_blk->GetBlockIdx(), 
                blockHash.substr(2, 10).c_str(),
                prevBlockHash.substr(2,10).c_str(),
                pow_blk->GetTimestamp());
        shadow_push_eventlog(buf);
    }
}


template <typename T>
void ListLedgerManager<T>::ReplaceLedger(typename std::list<T>::iterator orig_start, 
                                         typename std::list<T>::iterator orig_end, 
                                         typename std::vector<T>::iterator new_start, 
                                         typename std::vector<T>::iterator new_end) {
    typename std::list<T>::iterator it = list_ledger.erase(orig_start, orig_end);
    
    while (new_start != new_end) {
        list_ledger.insert(it, *new_start);

        auto pow_blk = dynamic_cast<POWBlock*>(&*new_start);
        if (pow_blk) {
            // append shadow log
            char buf[256];
            std::stringstream str, str2;
            std::ostringstream ss, ss2;
            str << pow_blk->GetBlockHash();
            ss << str.rdbuf();
            std::string blockHash = ss.str();

            str2 << pow_blk->GetPrevBlockHash();
            ss2 << str2.rdbuf();
            std::string prevBlockHash = ss2.str();
            sprintf(buf, "BlockAppend,%lu,%s,%s,%f", 
                    pow_blk->GetBlockIdx(), 
                    blockHash.substr(2, 10).c_str(),
                    prevBlockHash.substr(2,10).c_str(),
                    pow_blk->GetTimestamp());
            shadow_push_eventlog(buf);
        }

        std::cout << "longest chain block is appended" << "\n";
        std::cout << *new_start << "\n";

        new_start++;
    }
}

template <typename T>
void ListLedgerManager<T>::LoadLedgerFromFile() {
    boost::filesystem::path myFile = boost::filesystem::current_path() / ledger_filename;

    if (boost::filesystem::exists(myFile)) {
        boost::filesystem::ifstream ifs(myFile/*.native()*/);
        boost::archive::text_iarchive ta(ifs);

        ta >> list_ledger; // foo is empty until now, it's fed by myFile

        std::cout << "Read ledger from " << myFile << "\n";
    }
}

template <typename T>
void ListLedgerManager<T>::SaveLedgerToFile() {
    boost::filesystem::path myFile = boost::filesystem::current_path() / ledger_filename;
    boost::filesystem::ofstream ofs(myFile);
    boost::archive::text_oarchive ta(ofs);

    ta << list_ledger; 
    std::cout << "Wrote ledger to " << myFile << "\n";

}

