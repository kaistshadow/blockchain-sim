#ifndef MESSAGE_H
#define MESSAGE_H

#include <iostream>
#include <string>
#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/set.hpp>
#include <string>

class Message {
 public:
    virtual ~Message() {};
    virtual int GetType() = 0;
    static const int TEST_MESSAGE = 1;
    static const int TX_MESSAGE = 2;
    static const int CONSENSUS_MESSAGE = 3;
    static const int GOSSIPMODULE_MESSAGE = 4;

 private:

    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
    }
};


#endif
