#include "stellarconsensus.h"

StellarConsensus* StellarConsensus::instance = 0;

StellarConsensus* StellarConsensus::GetInstance() {
    if (instance == 0) {
        instance = new StellarConsensus();
    }

    return instance;
}

void StellarConsensus::ProcessQueue() {
    while (!msgQueue.empty()) {
        StellarConsensusMessage msg = msgQueue.front();
        
        if (msg.type == StellarConsensusMessage_INIT_QUORUM) {
            // init quorums and slices
            SCPInit initMsg = boost::get<SCPInit>(msg.msg);
            quorums = initMsg.quorums; // set quorums
            slices = initMsg.slices; // set slices

            std::cout << "quorum initialized as follows!" << "\n";
            std::cout << quorums << "\n";
            std::cout << slices << "\n";
            std::cout << "print ended" << "\n";
        }
        else if (msg.type == StellarConsensusMessage_NOMINATE) {
            // TODO
        }
        msgQueue.pop();
    }
}

