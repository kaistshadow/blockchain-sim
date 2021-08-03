// "Copyright [2021] <kaistshadow>"
#ifndef BLEEPLIB_SRC_BL2_PEER_CONNECTIVITY_ADDRADVERTISEMENT_H_
#define BLEEPLIB_SRC_BL2_PEER_CONNECTIVITY_ADDRADVERTISEMENT_H_

#include <boost/serialization/vector.hpp>
#include <vector>
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

} // namespace libBLEEP_BL

#endif // BLEEPLIB_SRC_BL2_PEER_CONNECTIVITY_ADDRADVERTISEMENT_H_
