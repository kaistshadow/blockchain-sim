#ifndef TEST_MESSAGE_H
#define TEST_MESSAGE_H

#include "Message.h"

#include <iostream>
#include <string>
#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/set.hpp>
#include <string>

class TestMessage: public Message {
 public:
    TestMessage() {}
    TestMessage(std::string name) {test_str = name;}
    virtual int GetType() {return TEST_MESSAGE;}

    virtual ~TestMessage() {}

    std::string test_str;

 private:
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Message>(*this);
        boost::serialization::void_cast_register<Message,TestMessage>();
        ar & test_str;
    }
};

#endif
