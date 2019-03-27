#include "Message.h"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/serialization/export.hpp>

using namespace libBLEEP;

// to avoid unregisterd class error
#include "Transaction.h"
BOOST_CLASS_EXPORT(SimpleTransaction);

// to avoid unregisterd class error
#include "Block.h"
BOOST_CLASS_EXPORT(Block);

std::ostream& libBLEEP::operator<<(std::ostream& os, const Message& msg) {
    if (msg.GetType() == "StringMessage") 
        os << msg.GetPayload();
    return os;
}

std::string libBLEEP::GetSerializedString(std::shared_ptr<Transaction> tx) {
    std::string serial_str;
    Transaction* tx_ptr = tx.get();
    // serialize obj into an std::string
    boost::iostreams::back_insert_device<std::string> inserter(serial_str);
    boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
    boost::archive::binary_oarchive oa(s);
    oa << tx_ptr;
    s.flush();
    return serial_str;
}

std::string libBLEEP::GetSerializedString(std::shared_ptr<SimpleTransaction> tx) {
    std::string serial_str;
    SimpleTransaction* tx_ptr = tx.get();
    // serialize obj into an std::string
    boost::iostreams::back_insert_device<std::string> inserter(serial_str);
    boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
    boost::archive::binary_oarchive oa(s);
    oa << tx_ptr;
    s.flush();
    return serial_str;
}

std::string libBLEEP::GetSerializedString(std::shared_ptr<Block> block) {
    std::string serial_str;
    Block* block_ptr = block.get();
    // serialize obj into an std::string
    boost::iostreams::back_insert_device<std::string> inserter(serial_str);
    boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
    boost::archive::binary_oarchive oa(s);
    oa << block_ptr;
    s.flush();
    return serial_str;
}

std::shared_ptr<Block> GetDeserializedBlock(std::string str) {
    Block* block;
    // wrap buffer inside a stream and deserialize string_read into obj
    boost::iostreams::basic_array_source<char> device(str.c_str(), str.size());
    boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
    boost::archive::binary_iarchive ia(s);
    ia >> block;

    return std::shared_ptr<Block>(block);
}
