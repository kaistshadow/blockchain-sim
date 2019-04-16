#include "MainEventManager.h"

using namespace libBLEEP;


MainEventManager::MainEventManager() {
    _libev_loop = EV_DEFAULT;
}

void MainEventManager::Wait() {

    while (true) {
        ev_run (_libev_loop, EVRUN_ONCE);

        if (_asyncEventTriggered)
            break;
    }
    _asyncEventTriggered = false; // reset
    return;
}

