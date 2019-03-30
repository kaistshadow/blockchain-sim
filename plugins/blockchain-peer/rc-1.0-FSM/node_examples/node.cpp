#include "mainmodules/MainEventManager.h"
#include "datamodules/Peer.h"

#include "utility/ArgsManager.h"

using namespace libBLEEP;

// Need to implement event loop library

// implement event loop's wait

// implement event loop's appendNewNeighborPeer, GenerateTx

int main(int argc, char *argv[]) {

    gArgs.ParseParameters(argc, argv);

    if (gArgs.IsArgSet("-?") || gArgs.IsArgSet("-h") ||  gArgs.IsArgSet("-help") || gArgs.IsArgSet("-version")) {
        std::string strUsage = gArgs.HelpMessage();
        std::cout << strUsage << "\n";
        return 0;
    }

    /* allocate mainEventManager */
    // MainEventManager mainEventManager("143.248.38.189");
    MainEventManager mainEventManager(gArgs.GetArg("-id", "noid"));

    /* connect to peer */
    // mainEventManager.AsyncConnectPeer(PeerId("143.248.38.37"));
    for (auto neighborPeerId : gArgs.GetArgs("-connect"))
        mainEventManager.AsyncConnectPeer(PeerId(neighborPeerId));

    while(true) {
        mainEventManager.Wait(); // main event loop (wait for next event)
        
        switch (mainEventManager.GetEventType()) {
        case AsyncEventEnum::none:
            std::cout << "invalid event is triggered. " << "\n";
            exit(-1);
        case AsyncEventEnum::CompleteAsyncConnectPeer:
            {
                std::cout << "event for connection complete for peer. " << "\n";
                PeerId peerId = mainEventManager.GetAsyncEventDataManager().GetConnectedPeerId();
                std::cout << "connected peerId : " << peerId.GetId() << "\n";
                // mainEventManager.SendMessage(message);
                break;
            }
        case AsyncEventEnum::ErrorAsyncConnectPeer:
            {
                std::cout << "AsyncConnectPeer got error(" << mainEventManager.GetAsyncEventDataManager().GetError() << ")" << "\n";
                // try again with timer
                PeerId peerId = mainEventManager.GetAsyncEventDataManager().GetRefusedPeerId();                
                mainEventManager.AsyncConnectPeer(peerId, 10);
                break;
            }
        }
    }
}
