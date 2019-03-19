#include "GlobalEvent.h"
#include "../utility/GlobalClock.h"
#include "../Configuration.h"
#include "../datamodules/Transaction.h"

struct ev_loop* GlobalEvent::loop;

std::map<int, SocketEventWatcher *> GlobalEvent::socketWatcherMap;

ev_periodic GlobalEvent::txgenWatcher;

ev_periodic GlobalEvent::printAndFlush;

void GlobalEvent::onAcceptSocketIO(EV_P_ ev_io *w, int revents) {
    puts("inet stream socket has become readable");

    // Obtain a wrapper for the socket event watcher
    SocketEventWatcher* watcher = (SocketEventWatcher*)w->data;
    
    handleNetworkClass->HandleAcceptSocketIO(watcher->GetSocketFD());
}

void GlobalEvent::onSendRecvSocketIO(EV_P_ ev_io *w, int revents) {
    // Obtain a wrapper for the socket event watcher
    SocketEventWatcher* watcher = (SocketEventWatcher*)w->data;

    if (revents & EV_READ) {
        handleNetworkClass->HandleRecvSocketIO(watcher->GetSocketFD());
    }
    else if (revents & EV_WRITE) {
        handleNetworkClass->HandleSendSocketIO(watcher->GetSocketFD());
    }
}

void GlobalEvent::onPeriodicTxInjection(EV_P_ ev_periodic *w, int revents) {
    int numTx = ((float)generateTxNum)/generateTxTime;
    while (numTx-- > 0) {
        boost::shared_ptr<Transaction> tx = handleTransactionClass->MakeRandomValidTransaction();
        handleNetworkClass->BroadcastMsg(tx.get());
        injectedTxNum++;
    }

    if (injectedTxNum == generateTxNum)
        ev_periodic_stop(EV_A_ w);

    return;
} 

void GlobalEvent::onPeriodicPrintAndFlush(EV_P_ ev_periodic *w, int revents) {
    std::cout << "Time:" << utility::GetGlobalClock() << "\n";
    std::flush(std::cout);
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
