#ifndef PIPE_MANAGER_H
#define PIPE_MANAGER_H

#include "../datamodules/Pipe.h"

class PipeManager {
 private:
    bool _initialized = false;

    /* io callbacks */
    void _PipeIOCallback (ev::io &w, int revents);

    /* event watcher */
    std::list<ev::io> _pipeIOWList;  // Use std::list to avoid shadow segmentation fault error.


    Pipe _pipe;
    PipeRecvBuffer _recvBuff;

 public:
    bool IsInitialized() { return _initialized; }
    void Init();

    PipeRecvBuffer& GetRecvBuff() { return _recvBuff; }
    int GetPipeWriteFD() { return _pipe.GetWritePipeFD(); }
};



#endif
