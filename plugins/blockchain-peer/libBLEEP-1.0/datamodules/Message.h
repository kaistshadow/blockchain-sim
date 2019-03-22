#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>

#include <memory>

#include "Block.h"
#include "Transaction.h"

namespace libBLEEP {
    enum class MessageTypeEnum {
        none,
        String,
        newBlock
    };


    class Message {
    private:
        // TODO : should include type, payload, and src, dest informations
        MessageTypeEnum _type;
        std::string _payload;

    public:

        Message() {}
        Message(std::string s) { _payload = s; _type = MessageTypeEnum::String; }
        Message(std::string payload, MessageTypeEnum type) { _payload = payload; _type = type; }
        MessageTypeEnum GetType() const { return _type; }
        std::string GetPayload() const { return _payload; }

    private: // boost serialization
        friend class boost::serialization::access;
        // When the class Archive corresponds to an output archive, the
        // & operator is defined similar to <<.  Likewise, when the class Archive
        // is a type of input archive the & operator is defined similar to >>
        template<class Archive>
            void serialize(Archive & ar, const unsigned int version) {
            ar & _type;
            ar & _payload;
        }
    };

    std::ostream& operator<<(std::ostream&, const Message&); // in order to overload the << operator



    // functions for serialization
    std::string GetSerializedString(std::shared_ptr<Transaction> tx);
    std::string GetSerializedString(std::shared_ptr<SimpleTransaction> tx);
    std::string GetSerializedString(std::shared_ptr<Block> msg);

}






#endif
