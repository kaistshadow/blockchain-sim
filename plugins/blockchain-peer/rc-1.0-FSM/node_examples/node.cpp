#include "mainmodules/MainEventManager.h"
#include "datamodules/Peer.h"
#include "datamodules/Message.h"

#include "utility/ArgsManager.h"


using namespace libBLEEP;

// Need to implement event loop library

// implement event loop's wait

// implement event loop's appendNewNeighborPeer, GenerateTx

// TODO(0331) : step-by-step visualization, implement asyncGenerateTx
// TODO(0331) : SendMessage, recvMessage
// TODO(0401) : Change PeerManager to PeerConnManager. Support disconnect API
// TODO(0401) : Refactoring for socket closeEvent handling (use recvMsg instead of recv?)



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
        case AsyncEventEnum::CompleteAsyncGenerateRandomTransaction:
            {
                std::cout << "random transaction generated" << "\n";
                boost::shared_ptr<Transaction> generatedTx = mainEventManager.GetAsyncEventDataManager().GetGeneratedTx();
                std::cout << *generatedTx << "\n";
            }
        case AsyncEventEnum::RecvMessage:
            {
                std::cout << "RecvMessage" << "\n";
                std::shared_ptr<Message> msg = mainEventManager.GetAsyncEventDataManager().GetReceivedMsg();
                MessageType messageType = msg->GetType();
                if (messageType == "newTx") {
                    boost::shared_ptr<Transaction> receivedTx = GetDeserializedTransaction(msg->GetPayload());
                    std::cout << *receivedTx << "\n";
                }
            }
        }
    }
}
