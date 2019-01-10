#ifndef GLOBAL_EVENT_H
#define GLOBAL_EVENT_H

#include <ev.h>
#include <map>

extern bool isProxy;

namespace GlobalEvent {
    // global event loop
    extern struct ev_loop *loop;

    // periodic watcher for tx generator
    extern ev_periodic txgenWatcher;

    // periodic watcher for fast log print
    extern ev_periodic printAndFlush;


    void onPeriodicTxInjection(EV_P_ ev_periodic *w, int revents);
    void onPeriodicPrintAndFlush(EV_P_ ev_periodic *w, int revents);
};


#endif
