#include <fcntl.h>
#include "ShadowPipe.h"

#include "shadow_interface.h"

ShadowPipe::ShadowPipe() {
    int result = shadow_pipe2(_shadowpipeFD, 0);
    if (result < 0) {
        perror("pipe ");
        exit(1);
    }
    fcntl(_shadowpipeFD[0], F_SETFL, O_NONBLOCK); /* Change the fd into non-blocking state. */ 
}
