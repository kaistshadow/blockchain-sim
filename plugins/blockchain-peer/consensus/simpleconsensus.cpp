#include "simpleconsensus.h"
#include "../blockchain/txpool.h"
#include "../blockchain/block.h"
#include "../p2p/p2pmessage.h"
#include "../p2p/gossipprotocol.h"
#include "../util/nodeinfo.h"

SimpleConsensus* SimpleConsensus::instance = 0;
SimpleConsensus* SimpleConsensus::GetInstance() {
    if (instance == 0) {
        instance = new SimpleConsensus(false);
    }
    return instance;
}

void SimpleConsensus::ProcessQueue() {
    while (!msgQueue.empty()) {
        SimpleConsensusMessage msg = msgQueue.front();
        
        if (msg.type == SimpleConsensusMessage_INIT_QUORUM) {
            std::string value = boost::get<std::string>(msg.value);
            node_id = value; // set id
            std::cerr << "Consensus: i'm selected as quorum and my id is : " << node_id << "\n";
        }
        else if (msg.type == SimpleConsensusMessage_LEADER_ELECTION) {
            std::string node_id = boost::get<std::string>(msg.value);
            if (NodeInfo::GetInstance()->GetHostId() == node_id) {
                isLeader = true;
                std::cerr << "Consensus: i'm elected as consensus leader!" << "\n";
            }
        }
        msgQueue.pop();
    }
}

/**
 * Simple consensus implementation
 * Leader (currently, nodeid 0) periodically selects 5 valid transactions and combines them as a block.
 */
void SimpleConsensus::RunConsensusProtocol() {

    double time_elapsed = difftime(time(0), start_time);
    if (isLeader && next_epoch_time <= difftime(time(0), start_time)) {
        // for every second, leader tries to proceed a consensus protocol
        // if (TxPool::GetInstance()->items.size() >= 5) {
        if (TxPool::GetInstance()->GetPendingTxNum() >= 5) {
            std::cout << "Consensus: Consensus on block" << "\n";

            std::list<Transaction> tx_list = TxPool::GetInstance()->GetTxs(5);
            TxPool::GetInstance()->RemoveTxs(tx_list);
            // std::list<Transaction>& txpool = TxPool::GetInstance()->items;
            // std::list<Transaction> tx_list;
            // tx_list.push_back(txpool[0]);
            // tx_list.push_back(txpool[1]);
            // tx_list.push_back(txpool[2]);
            // tx_list.push_back(txpool[3]);
            // tx_list.push_back(txpool[4]);
            // txpool.erase (txpool.begin(), txpool.begin()+5);
            
            Block block("consensus block", tx_list);
            std::cout << "Consensus: " << block;
            P2PMessage p2pmessage(P2PMessage_BLOCK, block);
            
	    SimpleGossipProtocol::GetInstance()->PushToUpperQueue(p2pmessage);
        }

        // set next time for doing consensus
        next_epoch_time = time_elapsed+1;
    }
    return;
}

