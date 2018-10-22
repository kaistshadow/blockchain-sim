template <typename T>
T* ListLedgerManager<T>::GetLastBlock() {
   if (list_ledger.size() == 0) 
       return nullptr;
   else
       return &list_ledger.back();
}


template <typename T>
void ListLedgerManager<T>::ReplaceLedger(typename std::list<T>::iterator pos, typename std::vector<T>::iterator start, typename std::vector<T>::iterator end) {
    list_ledger.erase(pos, list_ledger.end());
    while (start != end) {
        TxPool::GetInstance()->RemoveTxs(start->GetTransactions());
        list_ledger.push_back(*start);
        std::cout << "Longest chain block is appended" << "\n";
        std::cout << *start << "\n";
        start++;
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

template <typename T>
void ListLedgerManager<T>::DumpLedgerToJSONFile(std::string jsonfilename) {
   boost::property_tree::ptree root;
   for (auto& blk : list_ledger) {
       jsonSerializer.DumpToJson(root, blk);
   }
   std::string path = std::string("json_dump/") + SimpleGossipProtocol::GetInstance()->GetHostId();
   boost::filesystem::path dir(path.c_str());
   if (!boost::filesystem::exists(dir)) {
       boost::filesystem::create_directories(path.c_str());
   }
   boost::property_tree::write_json(path + "/" + jsonfilename, root);
   /* boost::property_tree::write_json(jsonfilename, root);  */
   return;
}
