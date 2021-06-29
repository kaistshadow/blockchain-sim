//
// Created by Hyunjin Kim on 2021/06/04.
//
#include <boost/archive/binary_oarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>

#include "POSBlock.h"
#include "../crypto/SHA256.h"

using namespace libBLEEP_BL;

void POSBlock::CalcHash() {
    unsigned char hash_out[32];
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (const unsigned char*)tx_hash.str().c_str(), tx_hash.str().size());
    sha256_update(&ctx, (const unsigned char*)prev_block_hash.str().c_str(), prev_block_hash.str().size());
    sha256_update(&ctx, (const unsigned char*)&_slot_no, sizeof(unsigned long));
    sha256_update(&ctx, (const unsigned char*)&_creator, sizeof(unsigned long));
    sha256_update(&ctx, (const unsigned char*)&timestamp, sizeof(double));
    sha256_final(&ctx, hash_out);
    libBLEEP::UINT256_t hash_out_256(hash_out, 32);
    SetBlockHash(hash_out_256);
}

void POSBlock::SetGenesisBlock() {
    SetPrevBlockHash(0);
    SetCreator(0);
    SetSlotNo(0);
    SetBlockIdx(0);
    SetTimestamp(0);

    std::cout << "genesis txhash:" << tx_hash.str() << "\n";

    CalcHash();
}

void POSBlock::SetTxHash() {
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

    tx_hash = libBLEEP::UINT256_t(tx_hash_buf, 32);
}

std::ostream& POSBlock::print(std::ostream& out) const {
    out << GetId() << ":" << "Block has following transactions" << "\n";
    const std::list<std::shared_ptr<SimpleTransaction> >& tx_list = GetTransactions();
    for (std::list<std::shared_ptr<SimpleTransaction> >::const_iterator it=tx_list.begin();
            it!=tx_list.end(); it++) {
        out << GetId() << ":" << *(*it) << "\n";
    }
    out << GetId() << ":" << "Block idx=" << GetBlockIdx() << ",";
    out << "Block creator=" << GetCreator() << ",";
    out << "Block slot number=" << GetSlotNo() << ",";
    out << "Block hash=" << GetBlockHash() << ",";
    out << "Prev Block hash=" << GetPrevBlockHash() << ",";
    out << "Timestamp=[" << GetTimestamp() << "]\n";

    return out;
}