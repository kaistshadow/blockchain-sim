#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

#include <memory>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "Peer.h"
#include "MessageObject.h"

#include "utility/GlobalClock.h"
#include "crypto/SHA256.h"

#define BLEEP_MAGIC "BLEEPmsg"
#define BLEEP_MAGIC_SIZE 8

namespace libBLEEP_BL {
    typedef std::string MessageType;

    class Message {
    private:
        PeerId _src;
        PeerId _dest;
        MessageType _type;
        std::shared_ptr<MessageObject> _ptrToObj;
        std::string _messageId; // TODO : should include message_hashid

        static std::string GenMessageHash(std::string msg){
            unsigned char digest[SHA256_BLOCK_SIZE];
            SHA256_CTX ctx;
            sha256_init(&ctx);
            sha256_update(&ctx, msg.c_str(), msg.size());
            sha256_final(&ctx, digest);
            char mdString[SHA256_BLOCK_SIZE*2+1];
            for (int i = 0; i < SHA256_BLOCK_SIZE; i++)
                sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
            return mdString;
        }

    public:
        Message() {}
        Message(PeerId src, PeerId dest, MessageType type) { _src = src; _dest = dest; _type = type; }
        Message(PeerId src, PeerId dest, MessageType type, std::shared_ptr<MessageObject> ptr) {
            _src = src; _dest = dest; _type = type; _ptrToObj = ptr;
            std::string timestamp = std::to_string(libBLEEP::GetGlobalClock());
            /* _messageId = GenMessageHash(src.GetId() + dest.GetId() + payload + timestamp); */

            // TODO : need to implement proper message hash
            _messageId = GenMessageHash(src.GetId() + dest.GetId() + timestamp);
        }
        MessageType GetType() const { return _type; }
        std::shared_ptr<MessageObject> GetObject() const { return _ptrToObj; }
        std::string GetMessageId() const { return _messageId; }
        PeerId GetSource() const { return _src; }
        PeerId GetDest() const { return _dest; }

        /* std::string GenMessageHash(std::string msg);         */
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
            ar & _ptrToObj;
            ar & _messageId;
        }

    };

}




#endif
