#ifndef INVENTORY_H
#define INVENTORY_H

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

}


#endif
