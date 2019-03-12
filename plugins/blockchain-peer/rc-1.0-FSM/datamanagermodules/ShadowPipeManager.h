#ifndef SHADOW_PIPE_MANAGER_H
#define SHADOW_PIPE_MANAGER_H

#include "../datamodules/ShadowPipe.h"

class ShadowPipeManager {
 private:
    bool _initialized = false;

    /* io callbacks */
    void _ShadowPipeIOCallback (ev::io &w, int revents);

    /* event watcher */
    std::list<ev::io> _pipeIOWList;  // Use std::list to avoid shadow segmentation fault error.


    ShadowPipe _shadowPipe;
    ShadowPipeRecvBuffer _recvBuff;

 public:
    bool IsInitialized() { return _initialized; }
    void Init();

    ShadowPipeRecvBuffer& GetRecvBuff() { return _recvBuff; }
};



#endif
