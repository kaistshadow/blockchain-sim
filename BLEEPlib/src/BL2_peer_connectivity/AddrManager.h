#ifndef ADDR_MANAGER_H
#define ADDR_MANAGER_H

#include "Address.h"

#include <map>
#include <vector>

#define NUM_BUCKET 1024
#define BUCKET_SIZE 64

namespace libBLEEP_BL {
    class AddrManager {
    private:
        int nIdCount; // TODO : need to reset at some point
        std::map<int, Address> mapAddr;
        int vvNew[NUM_BUCKET][BUCKET_SIZE];

        int GetNewBucket(Address& addr); 
        int GetBucketPosition(Address& addr, int Nbucket);

    public:
        AddrManager();

        
        void Add(Address addr);
        std::vector<Address> GetAddresses();
        // TODO : new outgoing connection based on addresstable
        std::shared_ptr<Address> SelectAddressFromTable(); // if duplicated connection?
        void PrintAddressTable();

    };
}

#endif
