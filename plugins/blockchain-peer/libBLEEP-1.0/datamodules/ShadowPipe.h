#ifndef SHADOW_PIPE_H
#define SHADOW_PIPE_H

#include <ev++.h>
#include <list>

namespace libBLEEP {


    enum class ShadowPipeEventEnum {
        none,
            readEvent,
            writeEvent
            };

    class ShadowPipeRecvBuffer {
    public:
        int message_len;
        int received_len;
        std::string recv_str;    

    };

    class ShadowPipe {
    private:
        /* shadow pipe fds */
        int _shadowpipeFD[2];

    public:
        ShadowPipe();

        int GetReadPipeFD() { return _shadowpipeFD[0]; }
        int GetWritePipeFD() { return _shadowpipeFD[1]; }
    };



}
#endif
