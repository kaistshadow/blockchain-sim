#include "ShadowPipeManager.h"

#include <iostream>

using namespace libBLEEP;

void libBLEEP::ShadowPipeManager::Init() {
    _pipeIOWList.emplace_back(); 
    ev::io& shadowpipeIOW = _pipeIOWList.back();
    shadowpipeIOW.set<ShadowPipeManager, &ShadowPipeManager::_ShadowPipeIOCallback> (this);
    shadowpipeIOW.start(_shadowPipe.GetReadPipeFD(), ev::READ);

    _initialized = true;
}


void libBLEEP::ShadowPipeManager::_ShadowPipeIOCallback(ev::io &w, int revents) {
    std::cout << "shadowpipe iocallback called!" << "\n";

    SetEventTriggered(true);
    SetEventTriggeredFD(w.fd);
    SetEventType(ShadowPipeEventEnum::readEvent);
}
