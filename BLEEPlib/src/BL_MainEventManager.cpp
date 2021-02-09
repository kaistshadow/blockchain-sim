#include "BL_MainEventManager.h"
#include "BL1_socket/SocketLayer_API.h"
#include "BL2_peer_connectivity/PeerConnectivityLayer_API.h"
#include "BL3_protocol/ProtocolLayer_API.h"
#include "utility/Assert.h"
#include "utility/GlobalClock.h"

using namespace libBLEEP_BL;

MainEventManager *MainEventManager::_instance = 0;

MainEventManager *MainEventManager::Instance() {
    if (_instance == 0) {
        libBLEEP::M_Assert(0, "Instance must be initialized first.");
    }
    return _instance;
}

void MainEventManager::InitInstance(AsyncEventEnum internalEventEnum) {
    libBLEEP::M_Assert(_instance == 0, "Instance must be initialized once.");
    _instance = new MainEventManager(internalEventEnum);
}

MainEventManager::MainEventManager(AsyncEventEnum internalEventEnum) {
    _libev_loop = EV_DEFAULT;
    _internalHandleEventEnum = internalEventEnum;
}

static void HandleAsyncEvent(AsyncEvent &event) {
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
    }

}

void MainEventManager::Wait() {
    while(!_eventQueue.empty()) {
        AsyncEvent event = _eventQueue.front();
        if (_internalHandleEventEnum < event.GetType())
            return; // The event should be handled externally. So return
        else {
            // The event should be handled internally, so pop the event from the queue
            // and transmit to the proper layer
            _eventQueue.pop();       
            HandleAsyncEvent(event);
        }
    }

    while (true) {
        std::cout << "before ev_run" << "\n";

        ev_run (_libev_loop, EVRUN_ONCE);

        libBLEEP::PrintTimespec("ev_run returned");

        while (!_eventQueue.empty()) {
            AsyncEvent event = _eventQueue.front();
            if (event.GetType() > _internalHandleEventEnum) {
                return;
                // The event should be handled externally. So return.
            }
            else {
                // The event should be handled internally, so pop the event from the queue
                // and transmit to the proper layer
                _eventQueue.pop();
                HandleAsyncEvent(event);
            }
        }
    }
}

void MainEventManager::PushAsyncEvent(AsyncEvent event) {
    _eventQueue.push(event);
    return;
}

AsyncEvent MainEventManager::PopAsyncEvent() {
    libBLEEP::M_Assert(!_eventQueue.empty(), "queue needs a element to pop");
    
    AsyncEvent element = _eventQueue.front();
    _eventQueue.pop();
    return element;
}
