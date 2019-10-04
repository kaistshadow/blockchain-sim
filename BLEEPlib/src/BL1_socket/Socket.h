#ifndef SOCKET_H
#define SOCKET_H

#include "../BL_MainEventManager.h"
#include <ev++.h>
#include <iostream>

#define DEFAULT_SOCKET_PORT 3456
#define BACKLOG 100     /* how many pending connections queue will hold */

namespace libBLEEP_BL {

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
    private:
        /* ListenSocketWatcher : Monitor the event from listen socket  */
        class ListenSocketWatcher {
        private:
            ev::io _watcher;

            void _listenSocketIOCallback (ev::io &w, int revents) {
                std::cout << "listen socket IO callback called!" << "\n";

                AsyncEvent event(AsyncEventEnum::SocketAccept);
                event.GetData().SetNewlyAcceptedSocket(w.fd);
                g_mainEventManager->PushAsyncEvent(event);
                std::cout << "listen socket IO : event pushed!" << "\n";
            }
        public:
            ListenSocketWatcher(int fd) {
                _watcher.set<ListenSocketWatcher, &ListenSocketWatcher::_listenSocketIOCallback> (this);
                _watcher.start(fd, ev::READ);
            }
        };

        std::unique_ptr<ListenSocketWatcher> _watcher;

    public:
        ListenSocket(int port = DEFAULT_SOCKET_PORT);

        int DoAccept(); /* do accept and return file descriptor */

        virtual ~ListenSocket();
        virtual SocketTypeEnum GetType() { return SocketTypeEnum::ListenSocket; }

    };

    /* class for non-blocking connecting socket */
    class ConnectSocket : public Socket {
    private:
        /* ConnectSocketWatcher : Monitor the event from connecting socket */
        class ConnectSocketWatcher {
        private:
            /* event watcher */
            ev::io _watcher;
            
            /* event io callback */
            void _connectSocketIOCallback (ev::io &w, int revents) {
                std::cout << "connect socket IO callback called!!!" << "\n";

                if (revents & EV_READ) {
                    std::cout << "invalid event is triggered for connecting socket. " << "\n";
                    exit(-1);
                }

                AsyncEvent event(AsyncEventEnum::SocketConnect);
                event.GetData().SetNewlyConnectedSocket(w.fd);
                g_mainEventManager->PushAsyncEvent(event);
            }
        public:
            ConnectSocketWatcher(int fd) {
                _watcher.set<ConnectSocketWatcher, &ConnectSocketWatcher::_connectSocketIOCallback> (this);
                _watcher.start(fd, ev::WRITE);
            }
        };

        std::unique_ptr<ConnectSocketWatcher> _watcher;

    public:
        ConnectSocket(std::string domain);

        virtual ~ConnectSocket() {}; // should not be closed. (same fd can be used for dataSocket)
        virtual SocketTypeEnum GetType() { return SocketTypeEnum::ConnectSocket; }
    };

}

#endif
