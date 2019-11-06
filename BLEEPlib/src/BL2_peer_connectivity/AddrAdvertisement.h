#ifndef ADDR_ADVERTISEMENT_H
#define ADDR_ADVERTISEMENT_H

#include <boost/serialization/vector.hpp>

#include "Address.h"
#include "MessageObject.h"

namespace libBLEEP_BL {
    // TODO : Make it BlockchainObject to enable deduplication
    class AddrAd : public MessageObject  {
    private:
        std::vector< Address > _vAddr;
    public:
        AddrAd() {}
        AddrAd(std::vector< Address > vAddr) { _vAddr = vAddr; }
        
        std::vector< Address >& GetVAddr() { return _vAddr; }

    private: // boost serialization
        friend class boost::serialization::access;
        // When the class Archive corresponds to an output archive, the
        // & operator is defined similar to <<.  Likewise, when the class Archive
        // is a type of input archive the & operator is defined similar to >>
        template<class Archive>
            void serialize(Archive & ar, const unsigned int version) {
            ar & boost::serialization::base_object<MessageObject>(*this);
            boost::serialization::void_cast_register<MessageObject,AddrAd>();
            ar & _vAddr;
        }

    };
    
}

#endif
