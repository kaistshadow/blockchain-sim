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
        
        /* Try to allocate new neighbor peer, and return the pointer for allocated PeerInfo.
           If the PeerInfo already exists for given PeerId, return the pointer for it. */
        std::shared_ptr<PeerInfo> AppendNewNeighborPeer(PeerId peer);

        /* Try to allocate new *connected* neighbor peer, and return the pointer for allocated PeerInfo.
           If the PeerInfo already exists for given PeerId, update the status of the PeerInfo 
           and return the pointer for it. */
        std::shared_ptr<PeerInfo> AppendConnectedNeighborPeer(PeerId peer, int socketfd);

        /* Update disconnected socket information for the peer */
        void UpdateNeighborSocketDisconnection(PeerId closedPeer);
        void UpdateNeighborSocketDisconnection(int closedSocketfd);

    };




}



#endif
