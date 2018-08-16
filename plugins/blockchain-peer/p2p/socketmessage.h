#ifndef NETWORK_MESSAGE_H
#define NETWORK_MESSAGE_H

#include <iostream>
#include <string>
#include <boost/serialization/string.hpp>

#include "p2pmessage.h"

class SocketMessage {
 private:
    int socket_fd;
    P2PMessage p2pMessage; 

    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & socket_fd;
        ar & p2pMessage;
    }

    int length;
    std::string network_payload;

 public:
    SocketMessage() {};
    
    P2PMessage& GetP2PMessage() { return p2pMessage; }
    int GetPayloadLength() { return length; }
    std::string GetPayload() { return network_payload; }

    void SetSocketfd(int sfd) { socket_fd = sfd; }
    void SetP2PMessage(P2PMessage msg) { p2pMessage = msg; }
    void SetPayload(std::string payload) { network_payload = payload; length = payload.size(); }
};

std::string GetSerializedString(SocketMessage msg);

SocketMessage GetDeserializedMsg(std::string str);

#endif
