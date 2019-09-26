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

// Private function from RandomGossipNetworkModule
std::set<Distance, DistanceCmp> genNeighborPeerSet(PeerId myId, std::vector<PeerId> &neighborPeerIds){
    std::set<Distance, DistanceCmp> neighborPeerIdSet;
    UINT256_t myHashId = myId.GetIdHash();
    for(auto peer : neighborPeerIds){
        UINT256_t peerHashId = peer.GetIdHash();
        UINT256_t distance = myHashId ^ peerHashId;
        neighborPeerIdSet.insert(Distance(distance, peer));
    }
    return neighborPeerIdSet;
}

void mutate (char * name, size_t size) {
    const char alphanum[] = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int r = rand() % (int)std::min(strlen(name) + 1, size - 1);
    char newChar = alphanum[rand() % (sizeof(alphanum) - 1)];
    if (r == (int)strlen(name)) {
        name[r+1] = '\0';
    }
    name[r] = newChar;
}

void  genPeerList(std::vector<PeerId> &lst, std::string myId, int maxPeerNum, std::string victimId){

    const char * current_config = "config-examples/current-config.xml";

    const char * generated_config = "config-examples/rc1-eventloop-random-eclipse.xml";

    tinyxml2::XMLDocument doc;
    auto errorId = doc.LoadFile(generated_config);
    // if the config has already been generated, just behave normally
    // i.e. return a vector of IDs of all nodes defined before this one
    if (!errorId) {
        // ===== comment this block if you don't want attacker nodes to make outgoing connections
        doc.LoadFile(current_config);
        for (tinyxml2::XMLElement * node = doc.FirstChildElement("shadow")->FirstChildElement("node");
            node != NULL; node = node->NextSiblingElement()) {
                const char * nodeId = node->Attribute("id");
                if (nodeId == myId) break;
                lst.push_back(PeerId(nodeId));
        }
        // =====
    }

    // Generate the config to run to perform the attack
    else {
        int connectPeerNum = 20; // Same maximum number as in main()

        auto errorId = doc.LoadFile(current_config);
        if (!errorId) {
            doc.SaveFile(generated_config);
            for (tinyxml2::XMLElement * node = doc.FirstChildElement("shadow")->FirstChildElement("node");
                node != NULL; node = node->NextSiblingElement()) {
                    const char * nodeId = node->Attribute("id");
                    if (nodeId == victimId) break;
                    // fill the vector with all the IDs the victim can connect to
                    lst.push_back(PeerId(nodeId));
            }
        }

        PeerId victim = PeerId(victimId);
        UINT256_t victimHash = victim.GetIdHash();
        auto peerSet = genNeighborPeerSet(victim, lst);
        UINT256_t minDist = peerSet.begin()->GetPeerDistance();

        tinyxml2::XMLElement * insertAfter = doc.FirstChildElement("shadow")->FirstChildElement("node");
        for (int i = 0; i < connectPeerNum ; i++) {
            char name[20];
            sprintf(name, "attacker%d", i);
            PeerId peer = PeerId(name);
            // this is how the set of peers to connect to is randomly generated
            // cf RandomGossipNetworkModule.cpp @ genNeighborPeerSet()
            UINT256_t distance = victimHash ^ peer.GetIdHash();
            while (distance < minDist) {
                mutate(name, sizeof(name));
                peer = PeerId(name);
                distance = victimHash ^ peer.GetIdHash();
            }
            std::string str(name);
            tinyxml2::XMLElement * attackerNode = doc.NewElement("node");
            attackerNode->SetAttribute("id", str.c_str());
            tinyxml2::XMLElement * application = doc.NewElement("application");
            char id[24] = "-id=";
            if (application != NULL) {
                application->SetAttribute("plugin", "ATTACKERNODE");
                application->SetAttribute("time", "0");
                application->SetAttribute("arguments", strcat(id, str.c_str()));
            }
            attackerNode->InsertFirstChild(application);
            doc.FirstChildElement("shadow")->InsertAfterChild(insertAfter, attackerNode);
            insertAfter = attackerNode;
        }
        doc.SaveFile(generated_config);
    }
}

int main(int argc, char *argv[]) {
    int maxPeerNum = 81;
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
    std::string victim = gArgs.GetArg("-victim", "bleep20");

    MainEventManager mainEventManager;
    RandomGossipNetworkModule randomNetworkModule(myId, &mainEventManager, fanOut);
    genPeerList(peerList, myId, maxPeerNum, victim);
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
