#include "PeerConnectivityLayer_API.h"

#include "AdvertisementManager.h"
#include "Message.h"
#include "AddrAdvertisement.h"

using namespace libBLEEP_BL;

void AdvertisementManager::SendAdToPeers() {
    std::cout << "periodic address advertisement" << "\n";
    std::cout << "size of _addrToSend:" << _addrToSend.size() << "\n";

    for (auto const& pair : _addrToSend) {
        std::cout << "send to " << pair.first.GetId() << "\n";
        std::shared_ptr<MessageObject> ptrToObj = std::make_shared<AddrAd>(pair.second);
        std::shared_ptr<Message> message = std::make_shared<Message>(pair.first, "ADDR", ptrToObj);
        g_PeerConnectivityLayer_API->SendMsgToPeer(pair.first, message);

        // TODO : optimize using move semantic?
        for (auto &addr : pair.second) {
            std::cout << "sent addr:" << addr.GetString() << "\n";
            // _addrKnown[pair.first].insert(addr);  // Update _addrKnown in PushAddr function.
        }
    }
    _addrToSend.clear();
}