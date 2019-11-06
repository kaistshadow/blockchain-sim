#ifndef ADVERTISEMENT_MANAGER_H
#define ADVERTISEMENT_MANAGER_H

#include "Address.h"
#include "Peer.h"

#include <ev++.h>
#include <map>
#include <vector>
#include <unordered_set>

#define ADDRESS_BROADCAST_INTERVAL 30

namespace libBLEEP_BL {
    class AdvertisementManager {
    private:
        Address _selfAddr;
        std::map<PeerId, std::vector< Address >, PeerIdCompare> _addrToSend;
        // TODO : Maybe, change to unordered_map which is more efficient for large data
        std::map<PeerId, std::unordered_set<Address>, PeerIdCompare> _addrKnown; // already known address
        // TODO : we need to convert _addrKnown as Bloom Filter (similar to Bitcoin)

        // send addr message to peers
        void SendAdToPeers();

    public:
        AdvertisementManager(Address addr) { 
            _selfAddr = addr; 
            _advTimer = std::unique_ptr<AdvertisementTimer>(new AdvertisementTimer(this, ADDRESS_BROADCAST_INTERVAL));
        }
        void AdvertiseLocal(PeerId dest) { _addrToSend[dest].push_back(_selfAddr); }

        void PushAddr(PeerId dest, Address addr) {
            if (!_addrKnown[dest].count(addr)) {
                _addrToSend[dest].push_back(addr); 
                _addrKnown[dest].insert(addr);
            }
        }

    private:
        class LocalAdvertisementTimer{
            // call AdvertiseLocal for all destination
        };

    private:
        class AdvertisementTimer {
        private:
            AdvertisementManager* _outer;
            ev::timer _timer;
            double _time;  
            void _timerCallback(ev::timer &w, int revents) {
                std::cout << "periodic address advertisement" << "\n";
                _outer->SendAdToPeers();
            }
        public:
            AdvertisementTimer(AdvertisementManager* outer, double time) {
                _outer = outer;
                _time = time;
                _timer.set<AdvertisementTimer, &AdvertisementTimer::_timerCallback>(this);
                _timer.set(1, time); //repeat every 'time' seconds
                // TODO : add a randomness for timer's 'time'
                _timer.start();
            }

            ~AdvertisementTimer() {
            }
        };
        
        std::unique_ptr<AdvertisementTimer> _advTimer;
    };
}

#endif
