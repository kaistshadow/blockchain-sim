#include "MainEventManager.h"
#include "../utility/Assert.h"

using namespace libBLEEP;


MainEventManager::MainEventManager() {
    _libev_loop = EV_DEFAULT;
}

void MainEventManager::Wait() {

    while (true) {
        ev_run (_libev_loop, EVRUN_ONCE);

        // std::cout << "ev_run returned" << "\n";
        // if (_asyncEventTriggered)
        //     break;
        if (!_eventQueue.empty())
            break;
    }
    // _asyncEventTriggered = false; // reset
    return;
}


void MainEventManager::PushAsyncEvent(AsyncEvent event) {
    _eventQueue.push(event);
    return;
}

AsyncEvent MainEventManager::PopAsyncEvent() {
    M_Assert(!_eventQueue.empty(), "queue needs a element to pop");
    
    AsyncEvent element = _eventQueue.front();
    _eventQueue.pop();
    return element;
}
