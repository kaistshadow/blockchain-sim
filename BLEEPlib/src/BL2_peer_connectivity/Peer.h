#ifndef PEER_H
#define PEER_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>

namespace libBLEEP_BL {
    class DataSocket;

    class PeerId {
    private:
        std::string _id; // unique identifier of the peer (e.g. : domain)
    public:
        PeerId() {}
        PeerId(std::string id) {
            _id = id;
        }

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


    enum class PeerType {
        none,
        IncomingPeer,
        OutgoingPeer,
    };

    class Peer {
    private:
        PeerId _id;
        PeerType _peerType;

        // If a peer is an outgoing peer, connecting socket is needed before peer initialization.
        int _connSocket;

        // datasocket will be assigned after the initialization of the socket is accomplished.
        std::shared_ptr<DataSocket> _dataSocket = nullptr;

        // Is this peer is alive? check with ping-pong
        // If pong message is received from this peer, set this peer as alive/
        // Otherwise, set this peer as not alive.
        bool _pongReceived = true;

    public:
        Peer(PeerId id, PeerType type)
                : _id(id), _peerType(type) {};

        Peer(PeerId id, PeerType type, int connSocket)
                : _id(id), _peerType(type), _connSocket(connSocket) {};

        Peer(PeerId id, PeerType type, std::shared_ptr<DataSocket> dataSocket)
                : _id(id), _peerType(type), _dataSocket(dataSocket) {};


        PeerId &GetPeerId() { return _id; }

        int GetConnSocket() { return _connSocket; }

        PeerType GetPeerType() { return _peerType; }

        /* get set method for data socket */
        void SetDataSocket(std::shared_ptr<DataSocket> dataSocket) { _dataSocket = dataSocket; }

        std::shared_ptr<DataSocket> GetDataSocket() { return _dataSocket; }

        bool IsActive() { return _dataSocket != nullptr; }

        void SetPongReceived(bool flag) { _pongReceived = flag; }

        bool IsPongReceived() { return _pongReceived; }
    };

}

#endif
