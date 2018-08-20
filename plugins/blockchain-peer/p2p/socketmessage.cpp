#include "socketmessage.h"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/variant.hpp>

using namespace boost::archive;


std::string GetSerializedString(SocketMessage msg) {
    std::string payload;
    // serialize obj into an std::string payload
    boost::iostreams::back_insert_device<std::string> inserter(payload);
    boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > os(inserter);
    boost::archive::binary_oarchive oa(os);
    oa << msg;

    // don't forget to flush the stream to finish writing into the buffer
    os.flush();

    // now you get to const char* with payload.data() or payload.c_str()
    return payload;
}


SocketMessage GetDeserializedMsg(std::string str) {
    SocketMessage msg;
    // wrap buffer inside a stream and deserialize string_read into obj
    boost::iostreams::basic_array_source<char> device(str.c_str(), str.size());
    boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
    boost::archive::binary_iarchive ia(s);
    ia >> msg;
    
    return msg;
}
