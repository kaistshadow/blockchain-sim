#include "mainmodules/MainEventManager.h"
#include "mainmodules/BasicNetworkModule.h"
#include "mainmodules/RandomGossipNetworkModule.h"

#include "datamodules/Peer.h"
#include "datamodules/Message.h"

#include "utility/ArgsManager.h"
#include "utility/Random.h"
#include "utility/tinyxml2.h"

#include <iostream>
#include <fstream>

using namespace libBLEEP;

void  genPeerList(std::vector<PeerId> &lst, std::string myId, int maxPeerNum){
    char current_config[50];
    sprintf(current_config, "config-examples/current-config.%d.xml", (int) getppid() );
    tinyxml2::XMLDocument doc;
    auto errorId = doc.LoadFile(current_config);
    if (!errorId) {
        for (tinyxml2::XMLElement * node = doc.FirstChildElement("shadow")->FirstChildElement("node");
            node != NULL; node = node->NextSiblingElement()) {
                const char * nodeId = node->Attribute("id");
                if (nodeId == myId) break;
                lst.push_back(PeerId(nodeId));
        }
    }
}

int main(int argc, char *argv[]) {
    int maxPeerNum = 500;
    int connectPeerNum = 20;
    int fanOut = 7;
    std::vector<PeerId> peerList;


    gArgs.ParseParameters(argc, argv);

    if (gArgs.IsArgSet("-?") || gArgs.IsArgSet("-h") ||  gArgs.IsArgSet("-help") || gArgs.IsArgSet("-version")) {
        std::string strUsage = gArgs.HelpMessage();
        std::cout << strUsage << "\n";
        return 0;
    }
    std::string myId = gArgs.GetArg("-id", "noid");

    MainEventManager mainEventManager;
    RandomGossipNetworkModule randomNetworkModule(myId, &mainEventManager, fanOut);
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
                    if (msg->GetType() == "TxInject") {
                        PeerId src = msg->GetSource();
                        PeerId dest = PeerId("DestAll");
                        std::string payload = msg->GetPayload();
                        std::shared_ptr<Message> nMsg = std::make_shared<Message>(src, dest, "newTx", payload);
                        char buf[256];
                        sprintf(buf, " NewTx from %s %s",
                                src.GetId().c_str(),
                                nMsg->GetMessageId().c_str());
                        shadow_push_eventlog(buf);
                        randomNetworkModule.MulticastMessage(nMsg);
                        randomNetworkModule.InsertMessageSet(nMsg->GetMessageId());
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
