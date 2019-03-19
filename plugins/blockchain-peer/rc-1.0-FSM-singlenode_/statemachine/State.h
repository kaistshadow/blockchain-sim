#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <type_traits>
#include <ev++.h>
#include <list>

enum class StateEnum {
    uninitializedState,
        idleState,
        exitState
};

std::ostream& operator << (std::ostream& os, const StateEnum& obj);



enum class LibevEventEnum {
    none,
    pipeReadEvent,
};

class IdleState {
 private:
    bool _initialized = false;

    /* io callbacks */
    void _PipeIOCallback (ev::io &w, int revents);

    /* event watcher */
    std::list<ev::io> _pipeIOWList;  // Use std::list to avoid shadow segmentation fault error.

    /* pipe fds */
    int _pipeFD[2];

    /* event-triggered fd */
    int _eventTriggeredFD;
    LibevEventEnum _eventType;

 public:
    bool IsInitialized() { return _initialized; }
    void Init();

    int GetEventTriggeredFD() { return _eventTriggeredFD; }
    LibevEventEnum GetEventType() { return _eventType; }

    /* for test */
    int GetPipeWriteFD() { return _pipeFD[1]; }
};

extern IdleState idleState;

#endif

