//
// Created by ilios on 21. 2. 1..
//
#include "BL_MainEventManager.h"
#include "BL1_socket/SocketLayer_API.h"
#include "BL2_peer_connectivity/PeerConnectivityLayer_API.h"

using namespace libBLEEP_BL;

typedef enum _nodetype {
    NODE_SERVER, NODE_CLIENT
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

// Testcase for ConnectPeer API
static int testConnectPeer(Nodetype t) {
    if (t == NODE_CLIENT) {
        /* client-side test logic */

        /* init BLEEP library components */
        MainEventManager::InitInstance();
        BL_SocketLayer_API::Instance();
        BL_PeerConnectivityLayer_API::InitInstance("client");

        /* client tries to connect to server using BLEEP libray API*/
        BL_PeerConnectivityLayer_API::Instance()->ConnectPeer(PeerId("server"));

        /* init finishing timer */
        EndTimer(5);
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

        /* init finishing timer */
        EndTimer(5);

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


// Testcase for ConnectPeer API (for void target)
static int testConnectPeerToVoid(Nodetype t) {
    if (t == NODE_CLIENT) {
        /* client-side test logic */

        /* init BLEEP library components */
        MainEventManager::InitInstance();
        BL_SocketLayer_API::Instance();
        BL_PeerConnectivityLayer_API::InitInstance("client");

        /* client tries to connect to server using BLEEP libray API*/
        BL_PeerConnectivityLayer_API::Instance()->ConnectPeer(PeerId("server"));

        /* init finishing timer */
        EndTimer(30);
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
                    if (eventQueue[0] != AsyncEventEnum::SocketConnectFailed) return -1;
                    if (eventQueue[1] != AsyncEventEnum::FinishTest) return -1;

                    /* Event sequence is valid, thus return 0 */
                    return 0;
            }
        }

    } else if (t == NODE_SERVER) {
        // server do nothing and immediately terminated
        return 0;
    }
}


// Testcase for DisconnectPeer API
static int testDisconnectPeer(Nodetype t) {
    if (t == NODE_CLIENT) {
        /* client-side test logic */

        /* init BLEEP library components */
        MainEventManager::InitInstance();
        BL_SocketLayer_API::Instance();
        BL_PeerConnectivityLayer_API::InitInstance("client");

        /* client tries to connect to server using BLEEP libray API*/
        if (!BL_PeerConnectivityLayer_API::Instance()->ConnectPeer(PeerId("server"))) {
            std::cout << "ConnectPeer failed" << "\n";
            return -1;
        }
        if (!BL_PeerConnectivityLayer_API::Instance()->DisconnectPeer(PeerId("server"))) {
            std::cout << "DisconnectPeer failed" << "\n";
            return -1;
        }

        /* init finishing timer */
        EndTimer(5);
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
                    if (eventQueue[0] != AsyncEventEnum::FinishTest) return -1;

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

        /* init finishing timer */
        EndTimer(5);

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
                    if (eventQueue[0] != AsyncEventEnum::FinishTest) return -1;

                    /* Event sequence is valid, thus return 0 */
                    return 0;
            }
        }
    }
}

// Testcase for peer terminated by exit
class ExitTimer {
private:
    ev::timer _exit_timer;

    void _exit_timercallback(ev::timer &w, int revents) {
        std::cout << "exitTimer activated!" << "\n";
        exit(0);
    }

public:
    ExitTimer(double time) {
        _exit_timer.set<ExitTimer, &ExitTimer::_exit_timercallback>(this);
        _exit_timer.set(time, 0.);
        _exit_timer.start();
        std::cout << "closeListenSocketTimer started" << "\n";
    }
};

static int testPeerWithExit(Nodetype t) {
    if (t == NODE_CLIENT) {
        /* client-side test logic */

        /* init BLEEP library components */
        MainEventManager::InitInstance();
        BL_SocketLayer_API::Instance();
        BL_PeerConnectivityLayer_API::InitInstance("client");

        /* client tries to connect to server using BLEEP libray API*/
        if (!BL_PeerConnectivityLayer_API::Instance()->ConnectPeer(PeerId("server"))) {
            std::cout << "ConnectPeer failed" << "\n";
            return -1;
        }


        /* init finishing timer */
        EndTimer(200);
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
                    // if (eventQueue[2] != AsyncEventEnum::SocketRecv) return -1; // expected but exit doesn't close socket
                    // if (eventQueue[3] != AsyncEventEnum::PeerSocketClose) return -1;  // expected but exit doesn't close socket
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

        /* init exiting timer */
        ExitTimer timer(5);

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
            }
        }
    }    
}

// Testcase for peer terminated with explicit close
class CloseTimer {
private:
    ev::timer _close_timer;

    void _close_timercallback(ev::timer &w, int revents) {
        std::cout << "closeListenSocketTimer activated, let's close listenSocket!" << "\n";
        for (auto id : BL_PeerConnectivityLayer_API::Instance()->GetNeighborPeerIds()) {
            BL_PeerConnectivityLayer_API::Instance()->DisconnectPeer(id);
        }
        delete BL_PeerConnectivityLayer_API::Instance();
        delete BL_SocketLayer_API::Instance();
        exit(0);
    }

public:
    CloseTimer(double time) {
        _close_timer.set<CloseTimer, &CloseTimer::_close_timercallback>(this);
        _close_timer.set(time, 0.);
        _close_timer.start();
        std::cout << "closeListenSocketTimer started" << "\n";
    }
};

static int testPeerWithCloseExit(Nodetype t) {
    if (t == NODE_CLIENT) {
        /* client-side test logic */

        /* init BLEEP library components */
        MainEventManager::InitInstance();
        BL_SocketLayer_API::Instance();
        BL_PeerConnectivityLayer_API::InitInstance("client");

        /* client tries to connect to server using BLEEP libray API*/
        if (!BL_PeerConnectivityLayer_API::Instance()->ConnectPeer(PeerId("server"))) {
            std::cout << "ConnectPeer failed" << "\n";
            return -1;
        }


        /* init finishing timer */
        EndTimer(200);
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
                    if (eventQueue[2] != AsyncEventEnum::SocketRecv) return -1; 
                    if (eventQueue[3] != AsyncEventEnum::PeerSocketClose) return -1;
                    if (eventQueue[4] != AsyncEventEnum::FinishTest) return -1;

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

        /* init exiting timer */
        CloseTimer timer(5);

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
            }
        }
    }
}


// Testcase for peer terminated with explicit stop(shadow's configuration)
static int testPeerWithStop(Nodetype t) {
    if (t == NODE_CLIENT) {
        /* client-side test logic */

        /* init BLEEP library components */
        MainEventManager::InitInstance();
        BL_SocketLayer_API::Instance();
        BL_PeerConnectivityLayer_API::InitInstance("client");

        /* client tries to connect to server using BLEEP libray API*/
        if (!BL_PeerConnectivityLayer_API::Instance()->ConnectPeer(PeerId("server"))) {
            std::cout << "ConnectPeer failed" << "\n";
            return -1;
        }


        /* init finishing timer */
        EndTimer(150);
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
                    // disconnecting peer by ping-pong mechanism
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
    } else if (testcase == "ConnectPeerToVoid") {
        testFunc = testConnectPeerToVoid;
    } else if (testcase == "DisconnectPeer") {
        testFunc = testDisconnectPeer;
    } else if (testcase == "PeerWithExit") {
        testFunc = testPeerWithExit;
    } else if (testcase == "PeerWithCloseExit") {
        testFunc = testPeerWithCloseExit;
    } else if (testcase == "PeerWithStop") {
        testFunc = testPeerWithStop;
    }

    /* select node type */
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
