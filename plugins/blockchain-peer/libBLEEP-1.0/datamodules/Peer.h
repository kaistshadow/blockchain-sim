#ifndef PEER_H
#define PEER_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>


namespace libBLEEP {


    class PeerId {
    private:
        std::string _id; // unique identifier of the peer (e.g : domain)

    public:
        PeerId() {}
        PeerId(std::string id) { _id = id; }

        std::string GetId() const { return _id; }

    private: // boost serialization
        friend class boost::serialization::access;
        // When the class Archive corresponds to an output archive, the
        // & operator is defined similar to <<.  Likewise, when the class Archive
        // is a type of input archive the & operator is defined similar to >>
        template<class Archive>
            void serialize(Archive & ar, const unsigned int version) {
            ar & _id;
        }

    };

    // define comparator in order to use PeerId as the key of std::map
    struct PeerIdCompare { 
        bool operator() (const PeerId& lhs, const PeerId& rhs) const {
            return lhs.GetId() < rhs.GetId();
            /* if (lhs.GetHostname() != rhs.GetHostname()) */
            /*     return lhs.GetHostname() < rhs.GetHostname(); */
            /* else */
            /*     return lhs.GetIP() < lhs.GetIP(); */
        }
    };

    enum class SocketStatus {
        none,
        SocketDisconnected,
        SocketConnected,
    };

    class PeerInfo {
    private:
        int _socketfd; // currently, we only support single data socket per peer
        SocketStatus _socketStatus;
        std::string _ipaddr = "";

    public:
        PeerInfo() {} 

        /* get methods */
        int GetSocketFD() const { return _socketfd; }
        std::string GetIP() const { return _ipaddr; }
        SocketStatus GetSocketStatus() const { return _socketStatus; }
        
        /* set methods */
        void SetSocketFD(int socketfd) { _socketfd = socketfd; }
        void SetIP(std::string ip) { _ipaddr = ip; }
        void SetSocketStatus(SocketStatus status) { _socketStatus = status; }
    };


}



#endif
