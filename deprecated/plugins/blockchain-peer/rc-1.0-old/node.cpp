#include <iostream>
#include <memory>
#include "Configuration.h"
#include "event/GlobalEvent.h"
#include "utility/NodeInfo.h"
#include "utility/ArgsManager.h"
#include "utility/GlobalClock.h"

// =================  Blockchain Node Configuration Start ===================

// general configuration
std::shared_ptr<HandleNetwork> handleNetworkClass;
std::shared_ptr<HandleTransaction> handleTransactionClass;
std::shared_ptr<HandleConsensus> handleConsensusClass;
bool amIFullNode = true;
bool amINetworkParticipantNode = false;
int generateTxNum = 0;
int injectedTxNum = 0;
int generateTxTime = 0;

// configurations for proxy-based network
bool amIProxyNode = false;

// configurations for gossip-based network
bool amIContactNode = false;
bool isGossipShutdown = false;
int shutdownTime = -1;

// Blockchain Consensus Configuration
int block_tx_num = 5;
int mining_time = 10;
std::string mining_time_dev = "2.0";

// Blockchain Transaction Configuration
TransactionType txType = EnumSimpleTransaction;

int main(int argc, char *argv[]) {
    gArgs.ParseParameters(argc, argv);

    if (gArgs.IsArgSet("-?") || gArgs.IsArgSet("-h") ||  gArgs.IsArgSet("-help") || gArgs.IsArgSet("-version")) {
        std::string strUsage = gArgs.HelpMessage();
        std::cout << strUsage << "\n";
        return 0;
    }

    if (gArgs.IsArgSet("-handlenetwork")) {
        std::string networktype = gArgs.GetArg("-handlenetwork","");
        if (networktype == "proxy") {
            std::cout << "networktype is given as proxy!" << "\n";
            handleNetworkClass = HandleNetwork::create(HANDLE_NETWORK_PROXYMODEL);
        }
        else if (networktype == "gossip") {
            std::cout << "networktype is given as gossip!" << "\n";
            handleNetworkClass = HandleNetwork::create(HANDLE_NETWORK_GOSSIPMODEL);
        }
        else { // default is proxy
            handleNetworkClass = HandleNetwork::create(HANDLE_NETWORK_PROXYMODEL);
        }
    } else { // default is proxy
        handleNetworkClass = HandleNetwork::create(HANDLE_NETWORK_PROXYMODEL);
    }

    if (gArgs.IsArgSet("-handletransaction")) {
        std::string handletransactiontype = gArgs.GetArg("-handletransaction","");
        if (handletransactiontype == "basic") {
            handleTransactionClass = HandleTransaction::create(HANDLE_TRANSACTION_BASICMODEL);
        }
        else // default is basic model
            handleTransactionClass = HandleTransaction::create(HANDLE_TRANSACTION_BASICMODEL);
    } else { // default is basic model
        handleTransactionClass = HandleTransaction::create(HANDLE_TRANSACTION_BASICMODEL);
    }

    if (gArgs.IsArgSet("-handleconsensus")) {
        std::string consensustype = gArgs.GetArg("-handleconsensus","");
        if (consensustype == "pow") {
            handleConsensusClass = HandleConsensus::create(HANDLE_CONSENSUS_POW);
        }
        else // default is POW model
            handleConsensusClass = HandleConsensus::create(HANDLE_CONSENSUS_POW);
    } else { // default is POW model
        handleConsensusClass = HandleConsensus::create(HANDLE_CONSENSUS_POW);
    }

    if (gArgs.IsArgSet("-txtype")) {
        std::string transactiontype = gArgs.GetArg("-txtype","");
        if (transactiontype == "simple") {
            txType = EnumSimpleTransaction;
        }
        else if (transactiontype == "useless") {
            txType = EnumUselessTransaction;
        }
        else // default is 'SimpleTransaction'
            txType = EnumSimpleTransaction;
    }


    if (gArgs.GetBoolArg("-networkparticipant", false)) {
        amIFullNode = false;
        amINetworkParticipantNode = true;
    } else {
        amIFullNode = true;
        amINetworkParticipantNode = false;
    }

    amIProxyNode = gArgs.GetBoolArg("-proxynode", false);

    amIContactNode = gArgs.GetBoolArg("-contactnode", false);
    isGossipShutdown = gArgs.GetBoolArg("-gossipshutdown", false);
    shutdownTime = gArgs.GetArg("-shutdownTime", -1);

    generateTxNum = gArgs.GetArg("-generatetx", 0);
    generateTxTime = gArgs.GetArg("-timegeneratetx", 0);

    block_tx_num = gArgs.GetArg("-blocktxnum", 5);
    mining_time = gArgs.GetArg("-miningtime", 10);
    mining_time_dev = gArgs.GetArg("-miningtimedev", "2.0");


    std::cout << "Testing node up" << "\n";

    // init NodeInfo : host ip address
    NodeInfo::GetInstance()->SetHostIP();

    // init GlobalClock
    utility::globalclock_start = std::chrono::high_resolution_clock::now();

    // Allocate event loop handler
    GlobalEvent::loop = ev_default_loop(0);

    // Register a tx generation watcher to event loop handler
    if (generateTxNum > 0) {
        ev_periodic_init(&GlobalEvent::txgenWatcher, GlobalEvent::onPeriodicTxInjection, 1, 1, 0);
        ev_periodic_start(GlobalEvent::loop, &GlobalEvent::txgenWatcher);
    }

    // Register a periodic callback for fast log print
    ev_periodic_init(&GlobalEvent::printAndFlush, GlobalEvent::onPeriodicPrintAndFlush, 0, 1, 0);
    ev_periodic_start(GlobalEvent::loop, &GlobalEvent::printAndFlush);


    // Join the P2P network
    int result = handleNetworkClass->JoinNetwork();
    if (result != 0) {
        std::cout << "Failed to join Network. Exit" << "\n";
        exit(-1);
    }

    // Start event loop, i.e., start the Blockchain State Machine by jumping into idle state.
    std::cout << "before ev_loop" << "\n";
    ev_loop(GlobalEvent::loop, 0);
}
