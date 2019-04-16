#include "mainmodules/MainEventManager.h"
#include "networkmodules/BasicNetworkModule.h"
#include "datamodules/Peer.h"
#include "datamodules/Message.h"

#include "utility/ArgsManager.h"
#include "utility/Assert.h"

using namespace libBLEEP;

// TODO : create randomly generate transaction, and send it to blockchain node

int main(int argc, char *argv[]) {

    gArgs.ParseParameters(argc, argv);

    if (gArgs.IsArgSet("-?") || gArgs.IsArgSet("-h") ||  gArgs.IsArgSet("-help") || gArgs.IsArgSet("-version")) {
        std::string strUsage = gArgs.HelpMessage();
        std::cout << strUsage << "\n";
        return 0;
    }

    /* allocate mainEventManager */
    // MainEventManager mainEventManager("143.248.38.189");
    // MainEventManager mainEventManager(gArgs.GetArg("-id", "noid"));
    MainEventManager mainEventManager;
    BasicNetworkModule basicNetworkModule(gArgs.GetArg("-id", "noid"), &mainEventManager);


    /* connect to peer */
    for (auto neighborPeerId : gArgs.GetArgs("-connect"))
        basicNetworkModule.AsyncConnectPeer(PeerId(neighborPeerId));

    if (gArgs.IsArgSet("-txgeninterval")) {
        basicNetworkModule.AsyncGenerateRandomTransaction(gArgs.GetArg("-txgeninterval", 0));
    }

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
                basicNetworkModule.AsyncConnectPeer(peerId, 10);
                break;
            }
        case AsyncEventEnum::CompleteAsyncGenerateRandomTransaction:
            {
                std::cout << "random transaction generated" << "\n";
                boost::shared_ptr<Transaction> generatedTx = mainEventManager.GetAsyncEventDataManager().GetGeneratedTx();

                std::cout << *generatedTx << "\n";

                PeerId myPeerId(gArgs.GetArg("-id", "noid"));
                std::string payload = GetSerializedString(generatedTx);
                for (auto neighborId : gArgs.GetArgs("-connect")) {
                    PeerId destPeerId(neighborId);
                    std::shared_ptr<Message> msg = std::make_shared<Message>(myPeerId, destPeerId, 
                                                                             "newTx", payload);
                    basicNetworkModule.UnicastMessage(destPeerId, msg);
                }

                // Call another request, i.e., periodically generate transaction
                basicNetworkModule.AsyncGenerateRandomTransaction(gArgs.GetArg("-txgeninterval", 0));
                break;
            }
        case AsyncEventEnum::RecvMessage:
            {
                std::cout << "RecvMessage" << "\n";
                M_Assert(0, "why client received message?");
                break;
            }
        case AsyncEventEnum::NewPeerConnected:
            {
                std::shared_ptr<PeerId> newConnectedNeighbor = mainEventManager.GetAsyncEventDataManager().GetNewlyConnectedPeer();
                std::cout << "NewPeerConnected requested from " << newConnectedNeighbor->GetId() << "\n";
                break;
            }
        case AsyncEventEnum::PeerDisconnected:
            {
                std::shared_ptr<PeerId> disconnectedNeighbor = mainEventManager.GetAsyncEventDataManager().GetDisconnectedPeerId();
                std::cout << "Disconnection requested from " << disconnectedNeighbor->GetId() << "\n";

                break;
            }
        }
    }
}
