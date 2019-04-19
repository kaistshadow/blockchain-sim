#include "mainmodules/MainEventManager.h"
#include "mainmodules/BasicNetworkModule.h"

#include "datamodules/Peer.h"
#include "datamodules/Message.h"

#include "utility/ArgsManager.h"


using namespace libBLEEP;

// Need to implement event loop library

// implement event loop's wait

// implement event loop's appendNewNeighborPeer, GenerateTx

// TODO(0331) : step-by-step visualization, implement asyncGenerateTx
// TODO(0331) : SendMessage, recvMessage
// TODO(0401) : Change PeerManager to PeerConnManager. Add NewPeerConnected event.
// TODO(0402) : Support disconnect API
// TODO(0402) : Refactoring for socket closeEvent handling (use recvMsg instead of recv?)

// v1.0
// TODO : Seperate API src,dest & msg src,dest (for broadcasting portability)
// TODO : statmachine regtest
// TODO : documentation for (Transaction, TxPool), (Block,LedgerManager), (PeerId, Message)
//        (MainEventManager_v1), utility ArgsManager

// TODO : connected neighbor peer already exists handling. -> remove redundent datasocket
// TODO : NewPeerConnected event

//

int main(int argc, char *argv[]) {
    // for testing DisconnectPeer API
    int receivedMessageCount = 0;

    gArgs.ParseParameters(argc, argv);

    if (gArgs.IsArgSet("-?") || gArgs.IsArgSet("-h") ||  gArgs.IsArgSet("-help") || gArgs.IsArgSet("-version")) {
        std::string strUsage = gArgs.HelpMessage();
        std::cout << strUsage << "\n";
        return 0;
    }

    /* allocate mainEventManager */
    MainEventManager mainEventManager;
    // MainEventManager mainEventManager(gArgs.GetArg("-id", "noid"));
    BasicNetworkModule basicNetworkModule(gArgs.GetArg("-id", "noid"), &mainEventManager);

    /* connect to peer */
    // mainEventManager.AsyncConnectPeer(PeerId("143.248.38.37"));
    for (auto neighborPeerId : gArgs.GetArgs("-connect"))
        basicNetworkModule.AsyncConnectPeer(PeerId(neighborPeerId));

    while(true) {
        mainEventManager.Wait(); // main event loop (wait for next event)

        while (mainEventManager.ExistAsyncEvent()) {
            AsyncEvent event = mainEventManager.PopAsyncEvent();

            switch (event.GetType()) {
            case AsyncEventEnum::none:
                std::cout << "invalid event is triggered. " << "\n";
                exit(-1);
            case AsyncEventEnum::CompleteAsyncConnectPeer:
                {
                    std::cout << "event for connection complete for peer. " << "\n";
                    PeerId peerId = event.GetData().GetConnectedPeerId();
                    std::cout << "connected peerId : " << peerId.GetId() << "\n";
                    // mainEventManager.SendMessage(message);
                    break;
                }
            case AsyncEventEnum::ErrorAsyncConnectPeer:
                {
                    std::cout << "AsyncConnectPeer got error(" << event.GetData().GetError() << ")" << "\n";
                    // try again with timer
                    PeerId peerId = event.GetData().GetRefusedPeerId();
                    basicNetworkModule.AsyncConnectPeer(peerId, 10);
                    break;
                }
            case AsyncEventEnum::CompleteAsyncGenerateRandomTransaction:
                {
                    std::cout << "random transaction generated" << "\n";
                    boost::shared_ptr<Transaction> generatedTx = event.GetData().GetGeneratedTx();
                    std::cout << *generatedTx << "\n";
                    break;
                }
            case AsyncEventEnum::RecvMessage:
                {
                    std::cout << "RecvMessage" << "\n";
                    std::shared_ptr<Message> msg = event.GetData().GetReceivedMsg();
                    MessageType messageType = msg->GetType();
                    if (messageType == "newTx") {
                        boost::shared_ptr<Transaction> receivedTx = GetDeserializedTransaction(msg->GetPayload());
                        std::vector<PeerId> dests = basicNetworkModule.GetNeighborPeerIds();
                        basicNetworkModule.MulticastMessage(dests, msg);
                        std::cout << *receivedTx << "\n";
                    }

                    // for testing DisconnectPeer API
                    receivedMessageCount++;
                    if (receivedMessageCount >= 5) {
                        for (auto neighborPeerId : gArgs.GetArgs("-connect"))
                            basicNetworkModule.DisconnectPeer(PeerId(neighborPeerId));
                    }
                    break;
                }
            case AsyncEventEnum::NewPeerConnected:
                {
                    std::shared_ptr<PeerId> newConnectedNeighbor = event.GetData().GetNewlyConnectedPeer();
                    std::cout << "NewPeerConnected requested from " << newConnectedNeighbor->GetId() << "\n";

                    break;
                }
            case AsyncEventEnum::PeerDisconnected:
                {
                    std::shared_ptr<PeerId> disconnectedNeighbor = event.GetData().GetDisconnectedPeerId();
                    std::cout << "Disconnection requested from " << disconnectedNeighbor->GetId() << "\n";

                    break;
                }
            }
        }
    }
}
