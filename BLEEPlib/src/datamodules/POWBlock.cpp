#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/device/array.hpp>

#include "POWBlock.h"
#include "../crypto/SHA256.h"

using namespace libBLEEP;

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
