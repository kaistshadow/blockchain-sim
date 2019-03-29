#include "mainmodules/MainEventManager.h"
#include "datamodules/Peer.h"

using namespace libBLEEP;

// Need to implement event loop library

// implement event loop's wait

// implement event loop's appendNewNeighborPeer, GenerateTx

// TODO (0328) : debug asyncConnect, 
// TODO (0328) : use PeerManager in MainEventManager, AsyncConnectPeer check peerManager, visualize InitMyPeerId, 
// TODO (0329) : visualize AppendConnectedNeighborPeer(called after AsyncConnect), implement ScheduleEvent
// TODO (0401) : implement SendMessage, RecvMessage, 
// TODO (0402) : visualize 

// TODO : shadow(client) interface + visualization


int main() {

    /* allocate mainEventManager */
    MainEventManager mainEventManager("bleep1");

    /* connect to peer */
    mainEventManager.AsyncConnectPeer(PeerId("bleep0"));


    while(true) {
        mainEventManager.Wait(); // main event loop (wait for next event)
        
        switch (mainEventManager.GetEventType()) {
        case AsyncEventEnum::none: 
            {
                std::cout << "invalid event is triggered. " << "\n";
                exit(-1);
            }
        case AsyncEventEnum::CompleteAsyncConnectPeer:
            {
                std::cout << "event for connection complete for peer. " << "\n";
                // get information of the async event
                PeerId peerId = mainEventManager.GetAsyncEventDataManager().GetConnectedPeerId();
                std::cout << "connected peerId : " << peerId.GetId() << "\n";
                break;
            }
        }
    }
}
