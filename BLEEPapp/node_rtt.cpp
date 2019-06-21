#include "mainmodules/MainEventManager.h"
#include "mainmodules/EstimateRTTModule.h"
#include "mainmodules/POWModule.h"
#include "mainmodules/TxGeneratorModule.h"

#include "datamodules/Peer.h"
#include "datamodules/Message.h"
#include "datamodules/POWBlock.h"

#include "datamanagermodules/TxPool.h"
#include "datamanagermodules/ListLedgerManager.h"

#include "utility/ArgsManager.h"
#include "utility/GlobalClock.h"

#include "shadow_interface.h"

using namespace libBLEEP;

#include <chrono>
#include <ctime>
#include <time.h>
#include <math.h>
#include <sys/time.h>

int mined_block_num = 0;


int main(int argc, char *argv[]) {
    gArgs.ParseParameters(argc, argv);

    if (gArgs.IsArgSet("-?") || gArgs.IsArgSet("-h") ||  gArgs.IsArgSet("-help") || gArgs.IsArgSet("-version")) {
        std::string strUsage = gArgs.HelpMessage();
        std::cout << strUsage << "\n";
        return 0;
    }

    /* allocate mainEventManager */
    MainEventManager mainEventManager;
    // MainEventManager mainEventManager(gArgs.GetArg("-id", "noid"));

    std::string myId = gArgs.GetArg("-id", "noid");
    double rttSendTime = std::stof(gArgs.GetArg("-rttstart", "0.0"));
    int sendNum = std::stoi(gArgs.GetArg("-rttnum", "0"));
    EstimateRTTModule estimateRTTModule(myId, rttSendTime, sendNum, &mainEventManager);

    // int maxPeerNum = 500;
    // genPeerList(peerList, myId, maxPeerNum);
    for (auto neighborPeerId : gArgs.GetArgs("-connect")) {
        estimateRTTModule.AsyncConnectPeer(PeerId(neighborPeerId));
    }


    // BasicNetworkModule basicNetworkModule(gArgs.GetArg("-id", "noid"), &mainEventManager);

    // transaction pool
    TxPool txPool;

    // ListLedgerManager
    ListLedgerManager<POWBlock> ledger;


    std::map<std::string, bool> reconnectTry;
    while(true) {
        mainEventManager.Wait(); // main event loop (wait for next event)
        
        PrintTimespec("mainEventManager.Wait returned");
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

                    if (reconnectTry[peerId.GetId()] == false) {
                        estimateRTTModule.AsyncConnectPeer(peerId, 1); // retry once
                        reconnectTry[peerId.GetId()] = true;
                    }
                    break;
                }
            case AsyncEventEnum::RecvMessage:
                {
                    std::cout << "RecvMessage" << "\n";
                    PrintTimespec("mainEventLoop AsyncEventEnum::recvMessage");
                    std::shared_ptr<Message> msg = event.GetData().GetReceivedMsg();
                    MessageType messageType = msg->GetType();

                    if (messageType == "RTTReq") { 
                        std::cout << "received Req message" << "\n";
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
