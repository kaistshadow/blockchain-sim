#ifndef GLOBAL_EVENT_H
#define GLOBAL_EVENT_H

#include <ev.h>
#include <map>

extern bool isProxy;

// Wrapper class for libev I/O event watcher
class SocketEventWatcher {
 private:
    int socketfd;


 public:
    ev_io watcher;
    SocketEventWatcher(int fd) {socketfd = fd;}
    int GetSocketFD() { return socketfd; }
    
    void InitEventWatcher(void (*Callback)(struct ev_loop*, ev_io*, int), int events);
    void SetEventWatcher(int events);
    void StartEventWatcher();
    void StopEventWatcher();
};

namespace GlobalEvent {
    // global event loop
    extern struct ev_loop *loop;

    // map socketfd to socket event watcher
    extern std::map< int, SocketEventWatcher*> socketWatcherMap;

    // periodic watcher for tx generator
    extern ev_periodic txgenWatcher;

    void onSendRecvSocketIO(EV_P_ ev_io *w, int revents);
    void onAcceptSocketIO(EV_P_ ev_io *w, int revents);

    void onPeriodicTxInjection(EV_P_ ev_periodic *w, int revents);
};


#endif
