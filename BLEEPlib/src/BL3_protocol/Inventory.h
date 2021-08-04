// "Copyright [2021] <kaistshadow>"

#ifndef BLEEPLIB_SRC_BL3_PROTOCOL_INVENTORY_H_
#define BLEEPLIB_SRC_BL3_PROTOCOL_INVENTORY_H_

#include <vector>
#include <boost/serialization/vector.hpp>
#include "../BL2_peer_connectivity/Address.h"
#include "../BL2_peer_connectivity/MessageObject.h"

namespace libBLEEP_BL {

class Inventory : public MessageObject {
 private:
    std::vector<int> _a;

 public:
    Inventory() {}

 private: // boost serialization
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<MessageObject>(*this);
        boost::serialization::void_cast_register<MessageObject,Inventory>();
        ar & _a;
    }
};

}  // namespace libBLEEP_BL


#endif  // BLEEPLIB_SRC_BL3_PROTOCOL_INVENTORY_H_
