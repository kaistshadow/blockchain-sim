#include "GlobalEvent.h"
#include "../utility/GlobalClock.h"
#include "../Configuration.h"
#include "../datamodules/Transaction.h"

struct ev_loop* GlobalEvent::loop;

ev_periodic GlobalEvent::txgenWatcher;

ev_periodic GlobalEvent::printAndFlush;

void GlobalEvent::onPeriodicTxInjection(EV_P_ ev_periodic *w, int revents) {
    int numTx = ((float)generateTxNum)/generateTxTime;
    while (numTx-- > 0) {
        boost::shared_ptr<Transaction> tx = handleTransactionClass->MakeRandomValidTransaction();
        handleNetworkClass->BroadcastMsg(tx.get());
        injectedTxNum++;

        if (injectedTxNum == generateTxNum) {
            ev_periodic_stop(EV_A_ w);
            break;
        }
    }

    return;
}

void GlobalEvent::onPeriodicPrintAndFlush(EV_P_ ev_periodic *w, int revents) {
    //std::cout << "Time:" << utility::GetGlobalClock() << "\n";
    std::flush(std::cout);
}


