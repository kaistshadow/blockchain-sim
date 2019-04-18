#ifndef LIST_LEDGER_MANAGER_H
#define LIST_LEDGER_MANAGER_H

#include <string>
#include <vector>
#include <queue>
#include <list>
#include <type_traits>
#include <memory>

#include "../datamodules/Block.h"
#include "../datamodules/POWBlock.h"
#include "../datamodules/Peer.h"
#include "TxPool.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "shadow_interface.h"

namespace libBLEEP {
    double GetGlobalClock(); // forward declaration

    template <typename T> 
        class ListLedgerManager {
        static_assert(std::is_base_of<Block, T>::value, "T must inherit from Block");
    protected:
        std::list<T> list_ledger;
        std::string ledger_filename;
        PeerId _myPeerId; // for shadow visualization & debugging
    public:
        ListLedgerManager() {}
        ListLedgerManager(PeerId myPeerId) { _myPeerId = myPeerId; }
        ListLedgerManager(std::string filename) { ledger_filename = filename; }
        ListLedgerManager(PeerId myPeerId, std::string filename) { ledger_filename = filename; _myPeerId = myPeerId; }

        /**
         * Set and Get a ledger
         */
        void SetLedger(std::list<T> blocks) { list_ledger = blocks; }
        std::list<T> &GetLedger() { return list_ledger; }

        T* GetLastBlock();
        unsigned long GetNextBlockIdx() { return list_ledger.size(); }
        void AppendBlock(std::shared_ptr<T> blk); 
        /* void AppendBlock(std::shared_ptr<T> blk); { list_ledger.push_back(*blk); } */

        // Replace a ledger
        void ReplaceLedger(typename std::list<T>::iterator orig_start, typename std::list<T>::iterator orig_end, 
                           typename std::vector<T>::iterator new_start, typename std::vector<T>::iterator new_end); 

        // Load ledger from file
        void LoadLedgerFromFile();
        void SaveLedgerToFile();

    };

#include "ListLedgerManager_impl.h"

}
#endif // LEDGERMANAGER_H
