// "Copyright [2021] <kaistshadow>"

#ifndef BLEEPLIB_SRC_BL2_PEER_CONNECTIVITY_ADDRESS_H_
#define BLEEPLIB_SRC_BL2_PEER_CONNECTIVITY_ADDRESS_H_
#include <boost/serialization/vector.hpp>

#include <string>

namespace libBLEEP_BL {
class Address {
 private:
    std::string _addr;
    double _timestamp;
    // TODO : based on timestamp (or additional info)
    // the removal of the address from addresstable should be implemented
 public:
    Address() {}
    Address(std::string addr) : _addr(addr) {}
    Address(std::string addr, double time) : _addr(addr), _timestamp(time) {}

    std::string GetString() const { return _addr; }
    void UpdateTimestamp(double timestamp) { _timestamp = timestamp; }

 private: // boost serialization
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
        ar & _addr;
    }

 public: // for unordered_set
    bool operator==(const Address &addr) const {
        if (_addr == addr.GetString()) {
            return true;
        } else { 
            return false;
        }
    }
};

} // namespace libBLEEP_BL

// in order to use Address as a key of associative container
namespace std {
    template<>
        struct hash<libBLEEP_BL::Address> {
        size_t operator()(const libBLEEP_BL::Address& obj) const {
            return hash<string>()(obj.GetString());
        }
    };
}


#endif // BLEEPLIB_SRC_BL2_PEER_CONNECTIVITY_ADDRESS_H_
