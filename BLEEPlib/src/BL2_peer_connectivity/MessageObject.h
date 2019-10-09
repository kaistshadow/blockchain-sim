#ifndef MESSAGE_OBJECT_H
#define MESSAGE_OBJECT_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

namespace libBLEEP_BL {
    class MessageObject {
    public:
        MessageObject() {};
    private:
        friend class boost::serialization::access;
        // When the class Archive corresponds to an output archive, the
        // & operator is defined similar to <<.  Likewise, when the class Archive
        // is a type of input archive the & operator is defined similar to >>
        template<class Archive>
            void serialize(Archive & ar, const unsigned int version) {
        }
    };
}

#endif
