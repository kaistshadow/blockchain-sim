#ifndef PIPE_MANAGER_H
#define PIPE_MANAGER_H

#include "../datamodules/Pipe.h"

#include <map>
#include <memory>

namespace libBLEEP {

    typedef unsigned int PipeID;

    class PipeManager {
    private:
        /* should be refactored to handle infinitely many allocations */
        unsigned int _id_counter = 0;

        /* event io callback */
        void _PipeIOCallback (ev::io &w, int revents);

        /* event watcher */ /* maybe we should use map data structure? */
        std::list<ev::io> _pipeIOWList; 

        std::map<PipeID, std::shared_ptr<Pipe> > _pipes;
        std::map<PipeID, std::shared_ptr<PipeRecvBuffer> > _pipe_recvBuffs;

        /* libev event management */
        bool _eventTriggered = false;
        int _eventTriggeredFD;
        PipeEventEnum _eventType;

    public:
        PipeID CreateNewPipe();
        std::shared_ptr<Pipe> GetPipe(PipeID id);

        /* PipeRecvBuffer& GetRecvBuff() { return _recvBuff; } */
        /* int GetPipeWriteFD() { return _pipe.GetWritePipeFD(); } */

        /* event set method */
        void SetEventTriggered(bool triggered) { _eventTriggered = triggered; }
        void SetEventTriggeredFD(int fd) { _eventTriggeredFD = fd; }
        void SetEventType(PipeEventEnum type) { _eventType = type; }
    
        /* event get method */
        bool IsEventTriggered() { return _eventTriggered; }
        int GetEventTriggeredFD() { return _eventTriggeredFD; }
        PipeEventEnum GetEventType() { return _eventType; }

    };

}

#endif
