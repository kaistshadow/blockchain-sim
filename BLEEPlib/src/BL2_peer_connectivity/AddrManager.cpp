#include "utility/UInt256.h"
#include "utility/GlobalClock.h"
#include "utility/Random.h"
#include "crypto/SHA256.h"

#include "AddrManager.h"
using namespace libBLEEP_BL;


// private methods
int AddrManager::GetNewBucket(Address& addr) {
    // h = H(domain address)
    // new_bucket = h % 1024

    std::cout << "getnewbucket" << "\n";
    unsigned char hash_buf[SHA256_BLOCK_SIZE];
    SHA256_CTX ctx;
    sha256_init(&ctx);
    std::string addrStr = addr.GetString();
    std::cout << "addrStr:" << addrStr << "\n";
    sha256_update(&ctx, addrStr.c_str(), addrStr.size());
    sha256_final(&ctx, hash_buf);
    libBLEEP::UINT256_t hash(hash_buf,32);
    hash = hash % 1024;

    
    uint64_t Nbucket = hash.getint();
    std::cout << "1024 mod result:" << Nbucket << "\n";
    return Nbucket;
}
int AddrManager::GetBucketPosition(Address& addr, int Nbucket) {
    // new_slot = H(domain address, new_bucket) % 64

    unsigned char hash_buf[SHA256_BLOCK_SIZE];
    SHA256_CTX ctx;
    sha256_init(&ctx);
    std::string addrStr = addr.GetString();
    sha256_update(&ctx, addrStr.c_str(), addrStr.size());
    sha256_update(&ctx, (unsigned char*)&Nbucket, sizeof(int));
    sha256_final(&ctx, hash_buf);
    libBLEEP::UINT256_t hash(hash_buf,32);
    hash = hash % 64;
    uint64_t bucketPos = hash.getint();
    std::cout << "bucket position:" << bucketPos << "\n";
    return bucketPos;
}

// public methods

AddrManager::AddrManager() {
    for (int bucket = 0; bucket < NUM_BUCKET; bucket++) {
        for (int entry = 0; entry < BUCKET_SIZE; entry++) {
            vvNew[bucket][entry] = -1;
        }
    }
    nIdCount = 0;
}

void AddrManager::Add(Address addr) {
    int bucket = GetNewBucket(addr);
    int bucketPosition = GetBucketPosition(addr, bucket);
            
    bool fInsert = (vvNew[bucket][bucketPosition] == -1);
    if (!fInsert) {
        // if there's already valid address in bucket
        // check whether it is same address or not
        // if it's the same address, update the timestamp.
        // otherwise just ignore the new address.
        // (TODO : replacement algorithm should be implemented)
        
        int existAddrId = vvNew[bucket][bucketPosition];
        if (mapAddr[existAddrId].GetString() == addr.GetString()) {
            mapAddr[existAddrId].UpdateTimestamp(libBLEEP::GetGlobalClock());
        }
            
        // // remove prev addr
        // mapAddr.erase(vvNew[bucket][bucketPosition]);

        // // overwrite
        // vvNew[bucket][bucketPosition] = addrId;
        // mapAddr[addrId] = addr;

    } else if (fInsert) {
        int addrId = nIdCount++;
        vvNew[bucket][bucketPosition] = addrId;
        mapAddr[addrId] = addr;
    }

}

std::vector<Address> AddrManager::GetAddresses() {
    // return randomly generated address vector
    std::vector<Address> vAddr;

    // get all ids
    std::vector<int> vIds;
    vIds.reserve(mapAddr.size());
    for (auto const& imap: mapAddr)
        vIds.push_back(imap.first);

    // random shuffle
    auto rng = *(libBLEEP::get_global_random_source().get_default_random_source());
    std::shuffle(std::begin(vIds), std::end(vIds), rng);

    for (unsigned int n = 0; n < vIds.size(); n++) {
        if (vAddr.size() >= 1000) // maximum size of address vector is 1000
            break;
                
        const Address& addr = mapAddr[vIds[n]];
        vAddr.push_back(addr);
    }

    return vAddr;
}

std::shared_ptr<Address> AddrManager::SelectAddressFromTable() {
    if (mapAddr.size() == 0)
        return nullptr;
    
    // get all ids
    std::vector<int> vIds;
    vIds.reserve(mapAddr.size());
    for (auto const& imap: mapAddr)
        vIds.push_back(imap.first);

    // random shuffle
    auto rng = *(libBLEEP::get_global_random_source().get_default_random_source());
    std::shuffle(std::begin(vIds), std::end(vIds), rng);
            
    return std::make_shared<Address>(mapAddr[vIds[0]]);
}

void AddrManager::PrintAddressTable() {
    std::cout << "stored addr num:" << mapAddr.size() << "\n";
    for (auto x : mapAddr) {
        std::cout << x.second.GetString() << "\n";
    }
}
