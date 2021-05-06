//
// Created by ilios on 21. 2. 1..
//
#include "BL_MainEventManager.h"
#include "BL1_socket/SocketLayer_API.h"
#include "BL2_peer_connectivity/PeerConnectivityLayer_API.h"
#include "BL3_protocol/ProtocolLayer_API.h"
#include "BL3_protocol/ProtocolLayerPoW.h"
#include "BL3_protocol/POWProtocolParameter.h"

using namespace libBLEEP_BL;

typedef enum _nodetype {
    NODE_SERVER, NODE_CLIENT, NODE_SERVER2
} Nodetype;

typedef int (*test_func)(Nodetype t);


// Push 'FinishTest' event, so that it terminates the entire experiment deliberately
void _timerCallback(ev::timer &w, int revents) {
    MainEventManager::Instance()->PushAsyncEvent(AsyncEvent(AsyncEventEnum::FinishTest));
    return;
}

// Initiate timer that indicates the end of experiment
static void EndTimer(int time) {
    static ev::timer _timer;
    _timer.set<_timerCallback>();
    _timer.set(time, 0);
    _timer.start();
}

// parameters for PoW
//double libBLEEP_BL::txGenStartAt = 10;
//double libBLEEP_BL::txGenInterval = 4;
//double libBLEEP_BL::miningtime = 2;
//int libBLEEP_BL::miningnodecnt = 2;

// Testcase for ConnectPeer API
static int testConnectPeer(Nodetype t) {
    if (t == NODE_CLIENT) {
        /* client-side test logic */

        /* init BLEEP library components */
        MainEventManager::InitInstance();
        BL_SocketLayer_API::Instance();
        BL_PeerConnectivityLayer_API::InitInstance("client");
        BL_ProtocolLayer_API::InitInstance("PoW");

        /* client tries to connect to server using BLEEP libray API*/
        BL_PeerConnectivityLayer_API::Instance()->ConnectPeer(PeerId("server"));

        /* init finishing timer */
        EndTimer(4);
        /* init event buffer which records all asynchronous events */
        std::vector<AsyncEventEnum> eventQueue;

        while (true) {
            MainEventManager::Instance()->Wait(); // main event loop (wait for next event)
            // loop returned
            AsyncEvent event = MainEventManager::Instance()->PopAsyncEvent();
            eventQueue.push_back(event.GetType());

            switch (event.GetType()) {
                case AsyncEventEnum::Layer1_Event_Start ... AsyncEventEnum::Layer1_Event_End:
                    BL_SocketLayer_API::Instance()->SwitchAsyncEventHandler(event);
                    break;
                case AsyncEventEnum::Layer2_Event_Start ... AsyncEventEnum::Layer2_Event_End:
                    BL_PeerConnectivityLayer_API::Instance()->SwitchAsyncEventHandler(event);
                    break;
                case AsyncEventEnum::FinishTest:
                    /* This event indicates that the test is over */
                    /* Thus, tries to check whether the eventQueue contains a valid event sequence */
                    auto new_end = std::remove_if(eventQueue.begin(), eventQueue.end(),
                                                  [](AsyncEventEnum &event) {
                                                      return event == AsyncEventEnum::SocketWrite;
                                                  });
                    /* Remove socketwrite event records for simplicity */
                    eventQueue.erase(new_end, eventQueue.end());

                    for (auto event : eventQueue) {
                        std::cout << (int) event << "\n";
                    }

                    /* Verify the event sequence */
                    if (eventQueue[0] != AsyncEventEnum::SocketConnect) return -1;
                    if (eventQueue[1] != AsyncEventEnum::PeerSocketConnect) return -1;
                    if (eventQueue[2] != AsyncEventEnum::FinishTest) return -1;

                    /* Event sequence is valid, thus return 0 */
                    return 0;
            }
        }

    } else if (t == NODE_SERVER) {
        /* server-side test logic */

        /* init BLEEP library components */
        MainEventManager::InitInstance();
        BL_SocketLayer_API::Instance();
        BL_PeerConnectivityLayer_API::InitInstance("server");
        BL_ProtocolLayer_API::InitInstance("PoW");

        /* init finishing timer */
        EndTimer(4);

        /* init event buffer which records all asynchronous events */
        std::vector<AsyncEventEnum> eventQueue;

        while (true) {
            MainEventManager::Instance()->Wait(); // main event loop (wait for next event)

            // loop returned
            AsyncEvent event = MainEventManager::Instance()->PopAsyncEvent();
            eventQueue.push_back(event.GetType());

            switch (event.GetType()) {
                case AsyncEventEnum::Layer1_Event_Start ... AsyncEventEnum::Layer1_Event_End:
                    BL_SocketLayer_API::Instance()->SwitchAsyncEventHandler(event);
                    break;
                case AsyncEventEnum::Layer2_Event_Start ... AsyncEventEnum::Layer2_Event_End:
                    BL_PeerConnectivityLayer_API::Instance()->SwitchAsyncEventHandler(event);
                    break;
                case AsyncEventEnum::FinishTest:
                    /* This event indicates that the test is over */
                    /* Thus, tries to check whether the eventQueue contains a valid event sequence */
                    auto new_end = std::remove_if(eventQueue.begin(), eventQueue.end(),
                                                  [](AsyncEventEnum &event) {
                                                      return event == AsyncEventEnum::SocketRecv;
                                                  });
                    /* Remove socketRecv event records for simplicity */
                    eventQueue.erase(new_end, eventQueue.end());
                    for (auto event : eventQueue) {
                        std::cout << (int) event << "\n";
                    }

                    /* Verify the event sequence */
                    if (eventQueue[0] != AsyncEventEnum::SocketAccept) return -1;
                    if (eventQueue[1] != AsyncEventEnum::PeerRecvNotifyPeerId) return -1;
                    if (eventQueue[2] != AsyncEventEnum::PeerRecvMsg) return -1; // GetADDR msg received
                    if (eventQueue[3] != AsyncEventEnum::PeerRecvMsg) return -1; // ADDR msg received
                    if (eventQueue[4] != AsyncEventEnum::FinishTest) return -1;

                    /* Event sequence is valid, thus return 0 */
                    return 0;
            }
        }
    }
}

// Testcase for Mining
// parameters for PoW
//int libBLEEP_BL::txNumPerBlock = 2;
//double libBLEEP_BL::txGenStartAt = 10;
//double libBLEEP_BL::txGenInterval = 4;
//double libBLEEP_BL::miningtime = 2;
//int libBLEEP_BL::miningnodecnt = 2;
static int testBlockInvPropagate(Nodetype t) {
    if (t == NODE_CLIENT) {
        /* client-side test logic */

        /* init BLEEP library components */
        MainEventManager::InitInstance();
        BL_SocketLayer_API::Instance();
        BL_PeerConnectivityLayer_API::InitInstance("client");
        BL_ProtocolLayer_API::InitInstance("PoW");
        POWProtocolParameter powparams;
        powparams.txGenStartAt = 10; powparams.txGenInterval = 4;
        powparams.miningtime = 2; powparams.miningnodecnt = 1;
        BL_ProtocolLayer_API::Instance()->InitiateProtocol(&powparams);

        /* client tries to connect to server using BLEEP libray API*/
        BL_PeerConnectivityLayer_API::Instance()->ConnectPeer(PeerId("server"));

        /* init finishing timer */
        EndTimer(21);
        /* init event buffer which records all asynchronous events */
        std::vector<AsyncEventEnum> eventQueue;

        while (true) {
            MainEventManager::Instance()->Wait(); // main event loop (wait for next event)
            // loop returned
            AsyncEvent event = MainEventManager::Instance()->PopAsyncEvent();
            eventQueue.push_back(event.GetType());

            switch (event.GetType()) {
                case AsyncEventEnum::Layer1_Event_Start ... AsyncEventEnum::Layer1_Event_End:
                    BL_SocketLayer_API::Instance()->SwitchAsyncEventHandler(event);
                    break;
                case AsyncEventEnum::Layer2_Event_Start ... AsyncEventEnum::Layer2_Event_End:
                    BL_PeerConnectivityLayer_API::Instance()->SwitchAsyncEventHandler(event);
                    break;
                case AsyncEventEnum::Layer3_Event_Start ... AsyncEventEnum::Layer3_Event_End:
                    BL_ProtocolLayer_API::Instance()->SwitchAsyncEventHandler(event);
                    break;
                case AsyncEventEnum::FinishTest:
                    /* This event indicates that the test is over */
                    /* Thus, tries to check whether the eventQueue contains a valid event sequence */
                    auto new_end = std::remove_if(eventQueue.begin(), eventQueue.end(),
                                                  [](AsyncEventEnum &event) {
                                                      return event == AsyncEventEnum::SocketWrite;
                                                  });
                    /* Remove socketwrite event records for simplicity */
                    eventQueue.erase(new_end, eventQueue.end());

                    for (auto event : eventQueue) {
                        std::cout << (int) event << "\n";
                    }

                    /* Verify the event sequence */
                    if (eventQueue[0] != AsyncEventEnum::SocketConnect) return -1;
                    if (eventQueue[1] != AsyncEventEnum::PeerSocketConnect) return -1;
                    if (eventQueue[2] != AsyncEventEnum::EmuBlockMiningComplete) return -1; //Emulated Block Mining
                    if (eventQueue[eventQueue.size()-1] != AsyncEventEnum::FinishTest) return -1;

                    /* Event sequence is valid, thus return 0 */
                    return 0;
            }
        }

    } else if (t == NODE_SERVER) {
        /* server-side test logic */
        POWProtocolParameter powparams;
        powparams.txGenStartAt = (double)INT_MAX;
        powparams.txGenInterval = (double)INT_MAX;
        powparams.miningtime = 10000; powparams.miningnodecnt = 2;

        /* init BLEEP library components */
        MainEventManager::InitInstance();
        BL_SocketLayer_API::Instance();
        BL_PeerConnectivityLayer_API::InitInstance("server");
        BL_ProtocolLayer_API::InitInstance("PoW");
        BL_ProtocolLayer_API::Instance()->InitiateProtocol(&powparams);

        /* init finishing timer */
        EndTimer(21);

        /* init event buffer which records all asynchronous events */
        std::vector<AsyncEventEnum> eventQueue;

        while (true) {
            MainEventManager::Instance()->Wait(); // main event loop (wait for next event)

            // loop returned
            AsyncEvent event = MainEventManager::Instance()->PopAsyncEvent();
            eventQueue.push_back(event.GetType());

            switch (event.GetType()) {
                case AsyncEventEnum::Layer1_Event_Start ... AsyncEventEnum::Layer1_Event_End:
                    BL_SocketLayer_API::Instance()->SwitchAsyncEventHandler(event);
                    break;
                case AsyncEventEnum::Layer2_Event_Start ... AsyncEventEnum::Layer2_Event_End:
                    BL_PeerConnectivityLayer_API::Instance()->SwitchAsyncEventHandler(event);
                    break;
                case AsyncEventEnum::Layer3_Event_Start ... AsyncEventEnum::Layer3_Event_End:
                    BL_ProtocolLayer_API::Instance()->SwitchAsyncEventHandler(event);
                    break;
                case AsyncEventEnum::FinishTest:
                    /* This event indicates that the test is over */
                    /* Thus, tries to check whether the eventQueue contains a valid event sequence */
                    auto new_end = std::remove_if(eventQueue.begin(), eventQueue.end(),
                                                  [](AsyncEventEnum &event) {
                                                      return event == AsyncEventEnum::SocketRecv;
                                                  });
                    /* Remove socketRecv event records for simplicity */
                    eventQueue.erase(new_end, eventQueue.end());
                    for (auto event : eventQueue) {
                        std::cout << (int) event << "\n";
                    }

                    /* Verify the event sequence */
                    if (eventQueue[0] != AsyncEventEnum::SocketAccept) return -1;
                    if (eventQueue[1] != AsyncEventEnum::PeerRecvNotifyPeerId) return -1;
                    if (eventQueue[2] != AsyncEventEnum::PeerRecvMsg) return -1; // GetADDR msg received
                    if (eventQueue[3] != AsyncEventEnum::PeerRecvMsg) return -1; // ADDR msg received
                    if (eventQueue[4] != AsyncEventEnum::PeerRecvMsg) return -1; // Block inv
                    if (eventQueue[5] != AsyncEventEnum::ProtocolRecvMsg) return -1; // Block inv
                    if (eventQueue[eventQueue.size()-1] != AsyncEventEnum::FinishTest) return -1;

                    /* Event sequence is valid, thus return 0 */
                    return 0;
            }
        }
    }
}

/* This test format is inspired from shadow test format (tcp) */
int main(int argc, char *argv[]) {
    int result = -1;

    if (argc < 3) {
        std::cout << "error: testcase and type not specified in args" << "\n";
        return -1;
    }

    std::string testcase = argv[1];
    std::string nodetype = argv[2];

    /* select test case */
    test_func testFunc;
    if (testcase == "ConnectPeer") {
        testFunc = testConnectPeer;
    } else if (testcase == "BlockInvPropagate") {
        testFunc = testBlockInvPropagate;
    }

    /* select node type */
    if (nodetype == "server") {
        result = testFunc(NODE_SERVER);
    } else if (nodetype == "client") {
        result = testFunc(NODE_CLIENT);
    } else if (nodetype == "server2") {
        result = testFunc(NODE_SERVER2);
    }

    if (result == 0) {
        std::cout << "########### test passed ############" << "\n";
    } else {
        std::cout << "########### test failed ############" << "\n";
    }
    return result;
}
