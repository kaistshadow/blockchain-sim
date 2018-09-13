#include <string.h>

#include "stellarconsensus.h"
#include "../p2p/p2pmessage.h"
#include "../p2p/gossipprotocol.h"
#include "../crypto/sha256.h"

StellarConsensus* StellarConsensus::instance = 0;

StellarConsensus* StellarConsensus::GetInstance() {
    if (instance == 0) {
        instance = new StellarConsensus();
    }

    return instance;
}

bool StellarConsensus::CheckQuorumThreshold_nominate(StellarConsensusValue v) {
    std::set<std::string> voting_nodes;
    for (auto it = latestMessages.begin(); it != latestMessages.end(); it++) {
        std::string node_id = it->first;
        SCPNominate latestMsg = it->second;
        
        if (latestMsg.voted.find(v) != latestMsg.voted.end())
            voting_nodes.insert(node_id);
    }

    // check whether the quorumthreshold is satisfied
    for (std::set<std::string> quorum : quorums.GetQuorums()) {
        if (quorum.find(my_node_id) == quorum.end())
            continue;

        bool check = true;
        for (std::string node_id : quorum) {
            if (voting_nodes.find(node_id) == voting_nodes.end()) {
                check = false;
                break;
            }
        }
        if (check) // exist quorum that all node_id is voting_nodes
            return true;
    }
    return false;
}

bool StellarConsensus::CheckBlockThreshold_nominate(StellarConsensusValue v) {

    // check whether the blocking threshold is satisfied 
    // check whether the value has been nominated by any node in quorum slice
    std::set<std::string> slice = slices.GetSlices()[my_node_id];
    
    for (std::string node_id : slice) {
        SCPNominate latestMsg = latestMessages[node_id];
        if (latestMsg.voted.find(v) != latestMsg.voted.end())
            return true;
    }
    return false;
}
void StellarConsensus::UpdateRoundLeader() {
    // temporary implementation
    // assume single round
    // TODO: implement cryptography library and real stellar priority functions.

    // select one node among quorum slice
    std::set<std::string> slice = slices.GetSlices()[my_node_id];
    unsigned int topPriority = 0;
    std::string topNode;

    for (std::string node_id : slice) {
        BYTE neighbor[SHA256_BLOCK_SIZE];
        unsigned int value;

        SHA256_CTX ctx;
        sha256_init(&ctx);
        sha256_update(&ctx, (const unsigned char *)node_id.c_str(), node_id.size());
        sha256_final(&ctx, neighbor);

        memcpy(&value, neighbor, 4);
        if (value > topPriority) {
            topPriority = value;
            topNode = node_id;
        }
    }
    
    round_leader = topNode;
    std::cout << "node id:" << topNode << " is selected as leader node at " << my_node_id << "\n";
    return;
}
bool StellarConsensus::CheckEcho(SCPNominate nominateMsg) {
    std::cout << "round_leader=[" << round_leader << "]\n";
    if (round_leader == "") 
        UpdateRoundLeader();

    if (nominateMsg.sender_id == round_leader) 
        return true;
}

void StellarConsensus::ProcessQueue() {
    while (!msgQueue.empty()) {
        StellarConsensusMessage msg = msgQueue.front();
        
        if (msg.type == StellarConsensusMessage_INIT_QUORUM) {
            // init quorums and slices
            SCPInit initMsg = boost::get<SCPInit>(msg.msg);
            quorums = initMsg.quorums; // set quorums
            slices = initMsg.slices; // set slices
            my_node_id = initMsg.node_id; // set node id

            std::cout << "quorum initialized as follows!!" << "\n";
            std::cout << "node id:" << my_node_id << "\n";
            std::cout << quorums << "\n";
            std::cout << slices << "\n";
            std::cout << "print ended" << "\n";
        }
        else if (msg.type == StellarConsensusMessage_NOMINATE) {
            SCPNominate nominateMsg = boost::get<SCPNominate>(msg.msg);
            std::cout << "received nominate message, sender_id=[" << nominateMsg.sender_id << "], my_id=[" << my_node_id << "]" << "\n";
            // 1. check whether the msg comes from quorum
            if (quorums.inSameQuorum( nominateMsg.sender_id, my_node_id)) {
                
                // 2. update state 'latestMessages'
                if (latestMessages.find(nominateMsg.sender_id) != latestMessages.end()) {
                    SCPNominate prevMsg = latestMessages[nominateMsg.sender_id];
                    if (prevMsg.voted.size() < nominateMsg.voted.size() ||
                        prevMsg.accepted.size() < nominateMsg.voted.size()) {
                        latestMessages[nominateMsg.sender_id] = nominateMsg;
                    }
                }
                else {
                    latestMessages[nominateMsg.sender_id] = nominateMsg;
                    // latestMessages.insert( std::pair<std::string, SCPNominate>( nominateMsg.sender_id, nominateMsg));
                }

                // 3. check whether the msg comes from priority nodes
                // TODO : need to implement priority (currently, temprorary implementation)
                bool echo = false;
                if (CheckEcho(nominateMsg)) {
                    echo = true;
                }
                
                if (echo) {
                    std::cout << "node id:" << my_node_id << ", received nominate message for echoing" << "\n";
            
                    // First, update state
                    votedValues.insert(nominateMsg.voted.begin(), nominateMsg.voted.end());
                    acceptedValues.insert(nominateMsg.accepted.begin(), nominateMsg.accepted.end());
                }
                
                // Second, check quorum threshold or block threshold
                // if quorum or block threshold is satisfied for votedvalue, insert it to accepted value
                // if quorum or block threshold is satisfied for acceptedvalue, insert it to candidate value
                bool exceedThreshold = false;
                std::vector< StellarConsensusValue > del_values;
                for (StellarConsensusValue v : votedValues) {
                    if (CheckQuorumThreshold_nominate(v)) {
                        acceptedValues.insert(v);
                        exceedThreshold = true;
                        del_values.push_back(v);
                        std::cout << "node id:" << my_node_id << ", exceed quorum threshold" << "\n";
                    }
                    else if (CheckBlockThreshold_nominate(v)) {
                        acceptedValues.insert(v);
                        exceedThreshold = true;
                        del_values.push_back(v);
                        std::cout << "node id:" << my_node_id << ", exceed block threshold" << "\n";
                    }
                }
                for (StellarConsensusValue v : del_values) {
                    votedValues.erase(v);
                }
                // for (StellarConsensusValue v : acceptedValues) {
                //     if (CheckQuorumThreshold_accept_nominate(v)) {
                //         candidateValues.insert(v);
                //     }
                //     else if (CheckBlockThreshold_accept_nominate(v)) {
                //         candidateValues.insert(v);
                //     }
                // }

                if (echo || exceedThreshold) {
                    // 4. send new nominate message
                    SCPNominate scpNominateMsg;
                    scpNominateMsg.sender_id = my_node_id;
                    scpNominateMsg.slotIndex = 1;
                    scpNominateMsg.voted.insert(votedValues.begin(), votedValues.end());
                    scpNominateMsg.accepted.insert(acceptedValues.begin(), acceptedValues.end());

                    StellarConsensusMessage consensusMsg;
                    consensusMsg.type = StellarConsensusMessage_NOMINATE;
                    consensusMsg.msg = scpNominateMsg;

                    P2PMessage p2pmessage(P2PMessage_STELLARCONSENSUSMESSAGE, consensusMsg);
                    p2pmessage.SetProcessedByNode(GetNodeId());
                    SimpleGossipProtocol::GetInstance()->PushToQueue(p2pmessage);
                    // send to P2P gossip protocol module!
                }
            }
        }
        msgQueue.pop();
    }
}

