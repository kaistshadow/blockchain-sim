#ifndef SHADOW_PIPE_MANAGER_H
#define SHADOW_PIPE_MANAGER_H

#include "../datamodules/ShadowPipe.h"

namespace libBLEEP {

    class ShadowPipeManager {
    private:
        bool _initialized = false;

        /* io callbacks */
        void _ShadowPipeIOCallback (ev::io &w, int revents);

        /* event watcher */
        std::list<ev::io> _pipeIOWList;  // Use std::list to avoid shadow segmentation fault error.


        /* there's only single shadow pipe */
        ShadowPipe _shadowPipe;
        ShadowPipeRecvBuffer _recvBuff;

        /* libev event management */
        bool _eventTriggered = false;
        int _eventTriggeredFD;
        ShadowPipeEventEnum _eventType;

    public:
        bool IsInitialized() { return _initialized; }
        void Init();

        ShadowPipeRecvBuffer& GetRecvBuff() { return _recvBuff; }

        /* event set method */
        void SetEventTriggered(bool triggered) { _eventTriggered = triggered; }
        void SetEventTriggeredFD(int fd) { _eventTriggeredFD = fd; }
        void SetEventType(ShadowPipeEventEnum type) { _eventType = type; }
    
        /* event get method */
        bool IsEventTriggered() { return _eventTriggered; }
        int GetEventTriggeredFD() { return _eventTriggeredFD; }
        ShadowPipeEventEnum GetEventType() { return _eventType; }
    };


}
#endif
