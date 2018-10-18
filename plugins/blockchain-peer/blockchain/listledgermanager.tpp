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
