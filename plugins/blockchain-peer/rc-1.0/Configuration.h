#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "p2pnetwork/HandleNetwork.h"
#include "transaction/HandleTransaction.h"

extern std::shared_ptr<HandleNetwork> handleNetwork;

extern std::shared_ptr<HandleTransaction> handleTransaction;

extern bool amIProxyNode;







#endif
