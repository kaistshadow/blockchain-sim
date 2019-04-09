template <typename T>
T* ListLedgerManager<T>::GetLastBlock() {
   if (list_ledger.size() == 0) 
       return nullptr;
   else
       return &list_ledger.back();
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

