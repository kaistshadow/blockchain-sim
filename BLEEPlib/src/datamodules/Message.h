#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

#include <memory>

#include "Block.h"
#include "Transaction.h"
#include "Peer.h"
#include "POWConsensusMessage.h"
#include "Inventory.h"
#include "../utility/GlobalClock.h"

namespace libBLEEP {
    typedef std::string MessageType;

    class Message {
    private:
        // TODO : should include type, payload, and src, dest informations
        PeerId _src;
        PeerId _dest;
        MessageType _type;
        std::string _payload;
        std::string _messageId;

    public:
        Message() {}
        Message(PeerId src, PeerId dest, MessageType type) { _src = src; _dest = dest; _type = type; }
        Message(PeerId src, PeerId dest, MessageType type, std::string payload) {
            _src = src; _dest = dest; _type = type; _payload = payload;
            std::string timestamp = std::to_string(GetGlobalClock());
            _messageId = GenMessageHash(src.GetId() + dest.GetId() + payload + timestamp);
        }
        MessageType GetType() const { return _type; }
        std::string GetPayload() const { return _payload; }
        std::string GetMessageId() const { return _messageId; }
        PeerId GetSource() const { return _src; }
        PeerId GetDest() const { return _dest; }

    private: // boost serialization
        std::string GenMessageHash(std::string msg);
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
            ar & _messageId;
        }
    };

    std::ostream& operator<<(std::ostream&, const Message&); // in order to overload the << operator



    // functions for serialization
    /* std::string GetSerializedString(std::shared_ptr<Transaction> tx); */
    std::string GetSerializedString(boost::shared_ptr<Transaction> tx);
    std::string GetSerializedString(std::shared_ptr<Block> msg);
    std::string GetSerializedString(POWConsensusMessage msg);
    std::string GetSerializedString(Inventory inv);

    boost::shared_ptr<Transaction> GetDeserializedTransaction(std::string str);
    std::shared_ptr<Block> GetDeserializedBlock(std::string str);
    POWConsensusMessage GetDeserializedPOWConsensusMessage(std::string str);
    Inventory GetDeserializedInventory(std::string str);

}






#endif
