#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>

#include <memory>

#include "Block.h"
#include "Transaction.h"
#include "Peer.h"

namespace libBLEEP {
    typedef std::string MessageType;

    class Message {
    private:
        // TODO : should include type, payload, and src, dest informations
        PeerId _src;
        PeerId _dest;
        MessageType _type;
        std::string _payload;

    public:
        Message() {}
        Message(PeerId src, PeerId dest, MessageType type) { _src = src; _dest = dest; _type = type; }
        Message(PeerId src, PeerId dest, MessageType type, std::string payload) { _src = src; _dest = dest; _type = type; _payload = payload; }
        MessageType GetType() const { return _type; }
        std::string GetPayload() const { return _payload; }
        PeerId GetSource() const { return _src; }
        PeerId GetDest() const { return _dest; }

    private: // boost serialization
        friend class boost::serialization::access;
        // When the class Archive corresponds to an output archive, the
        // & operator is defined similar to <<.  Likewise, when the class Archive
        // is a type of input archive the & operator is defined similar to >>
        template<class Archive>
            void serialize(Archive & ar, const unsigned int version) {
            ar & _src;
            ar & _dest;
            ar & _type;
            ar & _payload;
        }
    };

    std::ostream& operator<<(std::ostream&, const Message&); // in order to overload the << operator



    // functions for serialization
    /* std::string GetSerializedString(std::shared_ptr<Transaction> tx); */
    std::string GetSerializedString(boost::shared_ptr<Transaction> tx);
    std::string GetSerializedString(std::shared_ptr<Block> msg);

    boost::shared_ptr<Transaction> GetDeserializedTransaction(std::string str);
    std::shared_ptr<Block> GetDeserializedBlock(std::string str);

}






#endif
