#ifndef PEER_MANAGER_H
#define PEER_MANAGER_H

#include <map>
#include "../datamodules/Peer.h"

namespace libBLEEP {

    enum class SocketStatus {
        none,
        SocketDisconnected,
        SocketConnected,
    };

    class PeerInfo {
    private:
        int _socketfd; // currently, we only support single data socket per peer
        SocketStatus _socketStatus;

        int _socketfd_remote;  // we also support additional data socket requested by remote
        SocketStatus _socketStatus_remote;

    public:
        PeerInfo() {}

        /* get methods */
        int GetSocketFD() const { return _socketfd; }
        SocketStatus GetSocketStatus() const { return _socketStatus; }
        int GetSocketFDRemote() const { return _socketfd_remote; }
        SocketStatus GetSocketStatusRemote() const { return _socketStatus_remote; }

        /* set methods */
        void SetSocketFD(int socketfd) { _socketfd = socketfd; }
        void SetSocketStatus(SocketStatus status) { _socketStatus = status; }
        void SetSocketFDRemote(int socketfd) { _socketfd_remote = socketfd; }
        void SetSocketStatusRemote(SocketStatus status) { _socketStatus_remote = status; }

    };


    /* This class manages peer informations (including my own peerId).
       It maintains table which contains PeerInfo objects for all known neighbor peers.
       For each PeerInfo, only single copy should exist.
       Thus, this manager allocates PeerInfo object only once for each unique PeerId,
       then manages them through pointer.
       For each unique copy of PeerInfo objects,
       users can access them only through the smart pointers (i.e., std::shared_ptr).  */
    class PeerManager {
    private:
        std::shared_ptr<PeerId> _myPeerId = nullptr;
        std::map < PeerId, std::shared_ptr<PeerInfo>, PeerIdCompare> _neighborPeers;

    public:
        PeerManager() {}

        /* get set method for my PeerId */
        void InitMyPeerId(std::string id);
        std::shared_ptr<PeerId> GetMyPeerId();

        /* get neighbor PeerId */
        std::shared_ptr<PeerId> GetPeerIdBySocket(int socketfd);

        /* get method for neighbor Peer */
        std::shared_ptr<PeerInfo> GetPeerInfo(PeerId peer);
        std::shared_ptr<PeerInfo> GetPeerInfoBySocket(int socketfd);
        std::vector<PeerId> GetNeighborPeerIds();

        /* Try to allocate new neighbor peer, and return the pointer for allocated PeerInfo.
           If the PeerInfo already exists for given PeerId, return the pointer for it. */
        /* std::shared_ptr<PeerInfo> AppendNewNeighborPeer(PeerId peer); */

        /* Try to allocate new *connected* neighbor peer, and return the pointer for allocated PeerInfo.
           If the PeerInfo already exists for given PeerId, update the status of the PeerInfo
           and return the pointer for it. */
        std::shared_ptr<PeerInfo> AppendNeighborPeerConnectedByMyself(PeerId peer, int socketfd);

        /* Try to allocate new connected neighbor peer, where the connection is requested by remote peer.
           If the PeerInfo already exists for given PeerId, update the status of the PeerInfo
           and return the pointer for it */
        std::shared_ptr<PeerInfo> AppendNeighborPeerConnectedByRemote(PeerId peer, int socketfd);

        /* Update disconnected socket information for the peer */
        void UpdateNeighborSocketDisconnection(int closedSocketfd);

        /* state of the peer */
        bool HasEstablishedDataSocket(PeerId peer);
        int GetConnectedSocketFD(PeerId peer);
    };




}



#endif
