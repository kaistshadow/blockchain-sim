#include "ShadowPipeManager.h"
#include "../statemachine/StateMachine.h"

void ShadowPipeManager::Init() {
    _pipeIOWList.emplace_back(); 
    ev::io& shadowpipeIOW = _pipeIOWList.back();
    shadowpipeIOW.set<ShadowPipeManager, &ShadowPipeManager::_ShadowPipeIOCallback> (this);
    shadowpipeIOW.start(_shadowPipe.GetReadPipeFD(), ev::READ);

    _initialized = true;
}


void ShadowPipeManager::_ShadowPipeIOCallback(ev::io &w, int revents) {
    std::cout << "shadowpipe iocallback called!" << "\n";

    gStateMachine.fdEventNotifier.SetEventTriggered(true);
    gStateMachine.fdEventNotifier.SetEventTriggeredFD(w.fd);    
    gStateMachine.fdEventNotifier.SetEventType(FileDescriptorEventEnum::shadowPipeReadEvent);
}
