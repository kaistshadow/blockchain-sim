#ifndef PIPE_H
#define PIPE_H

#include <ev++.h>
#include <list>

namespace libBLEEP {

    enum class PipeEventEnum {
        none,
            readEvent,
            writeEvent
            };

    class PipeRecvBuffer {
    public:
        int message_len;
        int received_len;
        std::string recv_str;    

    };

    class Pipe {
    private:
        /* pipe fds */
        int _pipeFD[2];

    public:
        Pipe();

        int GetReadPipeFD() { return _pipeFD[0]; }
        int GetWritePipeFD() { return _pipeFD[1]; }
    };

}




#endif
