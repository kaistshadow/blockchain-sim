#include "mainmodules/MainEventManager.h"
#include "mainmodules/BasicNetworkModule.h"
#include "mainmodules/RandomGossipNetworkModule.h"

#include "datamodules/Peer.h"
#include "datamodules/Message.h"

#include "utility/ArgsManager.h"
#include "utility/Random.h"


using namespace libBLEEP;

void  genPeerList(std::vector<PeerId> &lst, std::string myId, int maxPeerNum){
    for(int i = 0; i < maxPeerNum; i++){
        char name[10];
        sprintf(name, "bleep%d", i);
        if(myId == name) break;
        lst.push_back(PeerId(name));
    }
}

int main(int argc, char *argv[]) {
    int maxPeerNum = 500;
    int connectPeerNum = 10;
    int maxMulticastingNum = 8;
    std::set<std::string> messageSet;
    std::vector<PeerId> peerList;


    gArgs.ParseParameters(argc, argv);

    if (gArgs.IsArgSet("-?") || gArgs.IsArgSet("-h") ||  gArgs.IsArgSet("-help") || gArgs.IsArgSet("-version")) {
        std::string strUsage = gArgs.HelpMessage();
        std::cout << strUsage << "\n";
        return 0;
    }
    std::string myId = gArgs.GetArg("-id", "noid");

    MainEventManager mainEventManager;
    RandomGossipNetworkModule randomNetworkModule(myId, &mainEventManager);
    genPeerList(peerList, myId, maxPeerNum);
    randomNetworkModule.AsyncConnectPeers(peerList, connectPeerNum);

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
                    std::cout << "event for connection complete for peer." << "\n";
                    PeerId peerId = event.GetData().GetConnectedPeerId();
                    std::cout << "connected peerId : " << peerId.GetId() << "\n";
                    break;
                }
            case AsyncEventEnum::ErrorAsyncConnectPeer:
                {
                    std::cout << "AsyncConnectPeer got error(" << event.GetData().GetError() << ")" << "\n";
                    // try again with timer
                    PeerId peerId = event.GetData().GetRefusedPeerId();
                    randomNetworkModule.AsyncConnectPeer(peerId, 10);
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
                    std::shared_ptr<Message> msg = event.GetData().GetReceivedMsg();
                    if(true == messageSet.insert(msg->GetMessageId()).second){
                        char buf[256];
                        sprintf(buf, " NewTx %s %s",
                                gArgs.GetArg("-id", "noid").c_str(),
                                msg->GetMessageId().c_str());
                        shadow_push_eventlog(buf);
                        std::vector<PeerId> dests =
                            randomNetworkModule.GetNeighborPeerIds();
                        if (dests.size() == 0) break;
                        auto idxs = GenRandomNumSet(dests.size(), maxMulticastingNum);
                        randomNetworkModule.MulticastMessage(dests, msg, idxs);
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
