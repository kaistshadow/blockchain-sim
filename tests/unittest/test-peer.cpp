//
// Created by ilios on 21. 2. 1..
//
#include "BL_MainEventManager.h"
#include "BL1_socket/SocketLayer_API.h"
#include "BL2_peer_connectivity/PeerConnectivityLayer_API.h"

using namespace libBLEEP_BL;

typedef enum _nodetype {
    NODE_SERVER, NODE_CLIENT
} nodetype;

typedef int (*test_func)(nodetype t);


void _timerCallback(ev::timer &w, int revents) {
    MainEventManager::Instance()->PushAsyncEvent(AsyncEvent(AsyncEventEnum::FinishTest));
    return;
}

static void EndTimer(int time) {
    static ev::timer _timer;
    _timer.set<_timerCallback>();
    _timer.set(time, 0);
    _timer.start();
}

static int testConnectPeer(nodetype t) {
    if (t == NODE_CLIENT) {
        MainEventManager::InitInstance();
        BL_SocketLayer_API::Instance();
        BL_PeerConnectivityLayer_API::InitInstance("client");

        BL_PeerConnectivityLayer_API::Instance()->ConnectPeer(PeerId("server"));

        EndTimer(30);
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
                    auto new_end = std::remove_if(eventQueue.begin(), eventQueue.end(),
                                                  [](AsyncEventEnum &event) {
                                                      return event == AsyncEventEnum::SocketWrite;
                                                  });
                    eventQueue.erase(new_end, eventQueue.end());
                    for (auto event : eventQueue) {
                        std::cout << (int) event << "\n";
                    }
                    if (eventQueue[0] != AsyncEventEnum::SocketConnect) return -1;
                    if (eventQueue[1] != AsyncEventEnum::PeerSocketConnect) return -1;
                    if (eventQueue[2] != AsyncEventEnum::FinishTest) return -1;
                    return 0;
            }
        }

    } else if (t == NODE_SERVER) {
        MainEventManager::InitInstance();
        BL_SocketLayer_API::Instance();
        BL_PeerConnectivityLayer_API::InitInstance("server");

        EndTimer(30);
        std::vector<AsyncEventEnum> eventQueue;

        while (true) {
            MainEventManager::Instance()->Wait(); // main event loop (wait for next event)

            // loop returned
            AsyncEvent event = MainEventManager::Instance()->PopAsyncEvent();

            switch (event.GetType()) {
                case AsyncEventEnum::Layer1_Event_Start ... AsyncEventEnum::Layer1_Event_End:
                    BL_SocketLayer_API::Instance()->SwitchAsyncEventHandler(event);
                    break;
                case AsyncEventEnum::Layer2_Event_Start ... AsyncEventEnum::Layer2_Event_End:
                    BL_PeerConnectivityLayer_API::Instance()->SwitchAsyncEventHandler(event);
                    break;
                case AsyncEventEnum::FinishTest:
                    for (auto event : eventQueue) {
                        std::cout << (int) event << "\n";
                    }
                    auto new_end = std::remove_if(eventQueue.begin(), eventQueue.end(),
                                                  [](AsyncEventEnum &event) {
                                                      return event == AsyncEventEnum::SocketRecv;
                                                  });
                    eventQueue.erase(new_end, eventQueue.end());
                    for (auto event : eventQueue) {
                        std::cout << (int) event << "\n";
                    }
                    if (eventQueue[0] != AsyncEventEnum::SocketAccept) return -1;
                    if (eventQueue[1] != AsyncEventEnum::PeerNotifyRecv) return -1;
                    if (eventQueue[2] != AsyncEventEnum::FinishTest) return -1;
                    return 0;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int result = -1;

    if (argc < 3) {
        std::cout << "error: testcase and type not specified in args" << "\n";
        return -1;
    }

    std::string testcase = argv[1];
    std::string nodetype = argv[2];

    test_func testFunc;
    if (testcase == "ConnectPeer") {
        testFunc = testConnectPeer;
    }

    if (nodetype == "server") {
        result = testFunc(NODE_SERVER);
    } else if (nodetype == "client") {
        result = testFunc(NODE_CLIENT);
    }

    if (result == 0) {
        std::cout << "########### test passed ############" << "\n";
    } else {
        std::cout << "########### test failed ############" << "\n";
    }
    return result;
}