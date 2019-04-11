#include "PipeManager.h"
#include "../utility/Assert.h"

#include <iostream>

using namespace libBLEEP;

PipeID libBLEEP::PipeManager::CreateNewPipe() {
    PipeID pipe_id = ++_id_counter;

    // new pipe
    std::shared_ptr<Pipe> new_pipe = std::make_shared<Pipe>();
    _pipes[pipe_id] = new_pipe;

    // new recvBuff
    std::shared_ptr<PipeRecvBuffer> new_pipeBuff = std::make_shared<PipeRecvBuffer>();
    _pipe_recvBuffs[pipe_id] = new_pipeBuff;
    
    // create event watcher for pipe
    _pipeIOWList.emplace_back(); 
    ev::io& pipeIOW = _pipeIOWList.back();
    pipeIOW.set<PipeManager, &PipeManager::_PipeIOCallback> (this);
    pipeIOW.start(new_pipe->GetReadPipeFD(), ev::READ);

    return pipe_id;
}

std::shared_ptr<Pipe> libBLEEP::PipeManager::GetPipe(PipeID id) {
    M_Assert(_pipes.find(id) != _pipes.end(), "No valid pipe exists");
    return _pipes[id];
}


void libBLEEP::PipeManager::_PipeIOCallback(ev::io &w, int revents) {
    std::cout << "pipe iocallback called!" << "\n";

    SetEventTriggered(true);
    SetEventTriggeredFD(w.fd);
    SetEventType(PipeEventEnum::readEvent);
    // need to implement for write event
}
