#include "PipeManager.h"
#include "../statemachine/StateMachine.h"

void PipeManager::Init() {
    _pipeIOWList.emplace_back(); 
    ev::io& pipeIOW = _pipeIOWList.back();
    pipeIOW.set<PipeManager, &PipeManager::_PipeIOCallback> (this);
    pipeIOW.start(_pipe.GetReadPipeFD(), ev::READ);

    _initialized = true;
}


void PipeManager::_PipeIOCallback(ev::io &w, int revents) {
    std::cout << "pipe iocallback called!" << "\n";

    gStateMachine.fdEventNotifier.SetEventTriggered(true);
    gStateMachine.fdEventNotifier.SetEventTriggeredFD(w.fd);    
    gStateMachine.fdEventNotifier.SetEventType(FileDescriptorEventEnum::pipeReadEvent);
}
