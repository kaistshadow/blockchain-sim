#ifndef SOCKET_H
#define SOCKET_H

#include <ev++.h>
#include <list>

#define DEFAULT_SOCKET_PORT 3456
#define BACKLOG 100     /* how many pending connections queue will hold */

namespace libBLEEP {

    enum class SocketEventEnum {
        none,
        readEvent,
        writeEvent
    };

    enum class SocketTypeEnum {
        none,
        ListenSocket,
        ConnectSocket,
        DataSocket
    };

    class Socket {
    protected:
        /* file descriptor */
        int _fd;

    public:
        virtual ~Socket() {};
        virtual SocketTypeEnum GetType() = 0;

        int GetFD() { return _fd; }
    };
    
    /* class for non-blocking listen socket */
    class ListenSocket : public Socket {
    public:
        ListenSocket(int port = DEFAULT_SOCKET_PORT);

        int DoAccept(); /* do accept and return file descriptor */

        virtual ~ListenSocket();
        virtual SocketTypeEnum GetType() { return SocketTypeEnum::ListenSocket; }
    };
    
    /* class for non-blocking connecting socket */
    class ConnectSocket : public Socket {
    public:
        ConnectSocket(std::string domain);

        virtual ~ConnectSocket() {};
        virtual SocketTypeEnum GetType() { return SocketTypeEnum::ConnectSocket; }
    };

    /* class for data-communicating socket */
    class DataSocket : public Socket {
    public:
        /* The DataSocket cannot be created from scratch.
           Every data socket are created after the ListenSocket or ConnectSocket 
           are created and estabilished. 
           Thus, constructor of the DataSocket receives the socket fd that
           is generated by ListenSocket or ConnectSocket */
        DataSocket(int fd) { _fd = fd; }

        virtual ~DataSocket();
        virtual SocketTypeEnum GetType() { return SocketTypeEnum::DataSocket; }
    };

}




#endif