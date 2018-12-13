#include "GlobalEvent.h"
#include "../Configuration.h"

struct ev_loop* GlobalEvent::loop;

std::map<int, SocketEventWatcher *> GlobalEvent::socketWatcherMap;

void GlobalEvent::onAcceptSocketIO(EV_P_ ev_io *w, int revents) {
    puts("inet stream socket has become readable");

    // Obtain a wrapper for the socket event watcher
    SocketEventWatcher* watcher = (SocketEventWatcher*)w->data;
    
    handleNetwork->HandleAcceptSocketIO(watcher->GetSocketFD());
}

void GlobalEvent::onSendRecvSocketIO(EV_P_ ev_io *w, int revents) {
    // Obtain a wrapper for the socket event watcher
    SocketEventWatcher* watcher = (SocketEventWatcher*)w->data;

    if (revents & EV_READ) {
        handleNetwork->HandleRecvSocketIO(watcher->GetSocketFD());
    }
    else if (revents & EV_WRITE) {
        handleNetwork->HandleSendSocketIO(watcher->GetSocketFD());
    }
}

void SocketEventWatcher::InitEventWatcher(void (*Callback)(struct ev_loop*, ev_io*, int), int events) {
    watcher.data = this;
    ev_io_init(&watcher, Callback, socketfd, events);
}

void SocketEventWatcher::StartEventWatcher() {
    ev_io_start(GlobalEvent::loop, &watcher);
}

void SocketEventWatcher::StopEventWatcher() {
    ev_io_stop(GlobalEvent::loop, &watcher);     
}

void SocketEventWatcher::SetEventWatcher(int events) {
    ev_io_set(&watcher, socketfd, events); 
}
