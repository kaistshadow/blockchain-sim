#include <unistd.h>
#include <fcntl.h>
#include "Pipe.h"

libBLEEP::Pipe::Pipe() {
    int result = pipe2(_pipeFD, 0);
    if (result < 0) {
        perror("pipe ");
        exit(1);
    }
    fcntl(_pipeFD[0], F_SETFL, O_NONBLOCK); /* Change the fd into non-blocking state. */ 
}
