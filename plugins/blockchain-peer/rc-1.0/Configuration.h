#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "p2pnetwork/HandleNetwork.h"
#include "transaction/HandleTransaction.h"

// =================  Blockchain Node Configuration Start ===================

// general configuration
extern std::shared_ptr<HandleNetwork> handleNetworkClass;
extern std::shared_ptr<HandleTransaction> handleTransactionClass;
extern bool amIFullNode;
extern bool amINetworkParticipantNode;

extern int generateTxNum; // total number of transaction which is generated and injected by a node
extern int generateTxTime; // total seconds for transaction injection.
extern int injectedTxNum;

// configurations for proxy-based network
extern bool amIProxyNode;  


// =================  Blockchain Node Configuration End =====================

// Blockchain Block Configuration 


// Blockchain Consensus Configuration


#endif
