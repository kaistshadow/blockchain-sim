#include "BasicNetworkModule.h"

namespace libBLEEP {


    class Distance{
        UINT256_t distance;
        PeerId peerId;

        public:
            Distance(UINT256_t distance, PeerId peerId)
                : distance(distance), peerId(peerId){}
            PeerId GetPeerId() const { return peerId; }
            UINT256_t GetPeerDistance() const { return distance; }

        friend struct DistanceCmp;

    };

    struct DistanceCmp{
        bool operator()(const Distance& d1, const Distance& d2) const{
            return d1.distance > d2.distance;
        }
    };


    class RandomGossipNetworkModule : public BasicNetworkModule {
        private:
            int _maxMulticastingNum;

            bool SendMulticastMsg(PeerId dest, std::shared_ptr<Message> message);

        public:
            /* Constructor with proper peer id */
            RandomGossipNetworkModule(std::string myPeerId, MainEventManager* worker, int maxMulticastingNum);

            bool MulticastMessage(std::shared_ptr<Message> message);

            std::vector<PeerId> GetNeighborPeerIds(PeerConnectMode mode = PeerConnectMode::none);

            PeerId GetMyPeerId();

            bool AsyncConnectPeers(std::vector<PeerId> &peerlist, int peerNum, int time = 0);

    };

}

