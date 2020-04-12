#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "p2pnetwork/HandleNetwork.h"
#include "transaction/HandleTransaction.h"
#include "consensus/HandleConsensus.h"
#include "datamodules/Transaction.h"

// =================  Blockchain Node Configuration Start ===================

// general configuration
extern std::shared_ptr<HandleNetwork> handleNetworkClass;
extern std::shared_ptr<HandleTransaction> handleTransactionClass;
extern std::shared_ptr<HandleConsensus> handleConsensusClass;
extern bool amIFullNode;
extern bool amINetworkParticipantNode;

extern int generateTxNum; // total number of transaction which is generated and injected by a node
extern int generateTxTime; // total seconds for transaction injection.
extern int injectedTxNum;

// configurations for proxy-based network
extern bool amIProxyNode;

// configurations for gossip-based network
extern bool amIContactNode;
extern bool isGossipShutdown;
extern int shutdownTime;
extern int eclipseTime;
extern int eclipseLogTime;

// =================  Blockchain Node Configuration End =====================

// Blockchain Block Configuration


// Blockchain Consensus Configuration
extern int block_tx_num;
extern int mining_time;
extern std::string mining_time_dev;


// Blockchain Transaction Configuration
extern TransactionType txType;

#endif
