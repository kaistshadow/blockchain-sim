#include <string.h>

#include "../util/eventqueue.h"
#include "../util/globalclock.h"

#include "stellarconsensus2.h"
#include "../p2p/p2pmessage.h"
#include "../p2p/gossipprotocol.h"
#include "../crypto/sha256.h"


StellarConsensus2* StellarConsensus2::instance = 0;

StellarConsensus2* StellarConsensus2::GetInstance() {
    if (instance == 0) {
        instance = new StellarConsensus2();
    }

    return instance;
}

void StellarConsensus2::StartNominationProtocol() {
    std::cout << utility::GetGlobalClock() << ":Nomination Protocol Round " << nomination_round << " Started for SCP" << "\n";

    ProceedNominationRound();
}

void StellarConsensus2::ProceedNominationRound() {
    nomination_round += 1;
    std::cout << utility::GetGlobalClock() << ":Nomination Protocol Round " << nomination_round << " Started for SCP" << "\n";

    Event e(nomination_round * 1000, [&]() {
            StellarConsensus2::GetInstance()->ProceedNominationRound();
        });
    EventQueueManager::GetInstance()->PushEvent(e);
}

void StellarConsensus2::ProcessQueue() {
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

                
                // Second, check quorum threshold or block threshold
                // if quorum or block threshold is satisfied for votedvalue, insert it to accepted value
                // if quorum or block threshold is satisfied for acceptedvalue, insert it to candidate value

                // 4. send new nominate message
                // send to P2P gossip protocol module!
            }
        }
        msgQueue.pop();
    }
}

