#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/device/array.hpp>

#include <time.h>

#include "POWBlock.h"
#include "../crypto/SHA256.h"


using namespace libBLEEP;

std::map<std::string, std::map<std::string, struct timespec> > libBLEEP::blocktimelogs;

// buf needs to store 30 characters
static int timespec2str(char *buf, uint len, const struct timespec *ts) {
    uint ret;
    struct tm t;

    tzset();
    if (localtime_r(&(ts->tv_sec), &t) == NULL)
        return 1;

    ret = strftime(buf, len, "%F %T", &t);
    if (ret == 0)
        return 2;
    len -= ret - 1;

    ret = snprintf(&buf[strlen(buf)], len, ".%09ld", ts->tv_nsec);
    if (ret >= len)
        return 3;

    return 0;
}

void libBLEEP::PrintBlockTimeLogs() {
    for (const auto &blkpair : blocktimelogs) {
        // std::cout << "PrintBlockTimeLogs:blockmsgid=" << blkpair.first << "\n";
        for (const auto &times : blkpair.second) {
            const uint TIME_FMT = strlen("2012-12-31 12:59:59.123456789") + 1;
            char timestr[TIME_FMT];
            if (timespec2str(timestr, sizeof(timestr), &times.second) != 0) {
                std::cout << "timespec2str failed" << "\n";
                exit(-1);
            }
            std::cout << "PrintBlockTimeLogs," << blkpair.first << "," << times.first << "," << timestr << "\n";
        }
    }
}

void POWBlock::SetTxHash() {
    // TODO: build merkle tree & calculate root hash
    // currently, just simple serialization & hashing

    std::string serialized_str;
    // serialize obj into an std::string serialized_str
    boost::iostreams::back_insert_device<std::string> inserter(serialized_str);
    boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > os(inserter);
    boost::archive::binary_oarchive oa(os);
    oa << GetTransactions();

    // don't forget to flush the stream to finish writing into the buffer
    os.flush();

    // now you get to const char* with payload.data() or payload.c_str()

    unsigned char tx_hash_buf[32];

    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, serialized_str.c_str(), serialized_str.size());
    sha256_final(&ctx, tx_hash_buf);

    tx_hash = UINT256_t(tx_hash_buf, 32);
}

std::ostream& POWBlock::print(std::ostream& out) const {
    out << GetId() << ":" << "Block has following transactions" << "\n";
    const std::list<boost::shared_ptr<Transaction> >& tx_list = GetTransactions();
    for (std::list<boost::shared_ptr<Transaction> >::const_iterator it=tx_list.begin();
         it!=tx_list.end(); it++) {
        out << GetId() << ":" << *(*it) << "\n";
    }
    out << GetId() << ":" << "Block idx=" << GetBlockIdx() << ",";
    out << "Block nonce=" << GetNonce() << ",";    
    out << "Block hash=" << GetBlockHash() << ",";    
    out << "Prev Block hash=" << GetPrevBlockHash() << ","; 
    out << "Timestamp=[" << GetTimestamp() << "],";    
    out << "Difficulty=" << GetDifficulty() << "\n";    

    return out;
}

// std::ostream& libBLEEP::operator<<(std::ostream& os, const POWBlock& blk) {
//     os << blk.GetId() << ":" << "Block has following transactions" << "\n";
//     const std::list<boost::shared_ptr<Transaction> >& tx_list = blk.GetTransactions();
//     for (std::list<boost::shared_ptr<Transaction> >::const_iterator it=tx_list.begin();
//          it!=tx_list.end(); it++) {
//         os << blk.GetId() << ":" << *(*it) << "\n";
//     }
//     os << blk.GetId() << ":" << "Block idx=" << blk.GetBlockIdx() << ",";
//     os << "Block nonce=" << blk.GetNonce() << ",";    
//     os << "Block hash=" << blk.GetBlockHash() << ",";    
//     os << "Prev Block hash=" << blk.GetPrevBlockHash() << ","; 
//     os << "Timestamp=[" << blk.GetTimestamp() << "],";    
//     os << "Difficulty=" << blk.GetDifficulty() << "\n";    

//     return os;
// }
