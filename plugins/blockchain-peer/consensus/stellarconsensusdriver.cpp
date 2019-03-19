#include <iostream>

#include "stellarconsensusdriver.h"
#include "stellarconsensus2.h"

bool StellarConsensusDriver::HasNewConsensus() {
    // 
    return false;
}

void StellarConsensusDriver::UpdateLedger() {
    //
}

void StellarConsensusDriver::TriggerNextConsensus() {


    // if (lastPrepareTime) {
    //     double elapsed = difftime(time(0), lastPrepareTime);
    //     std::cout << elapsed << "\n";
    //     if ( elapsed >= 5 ) { 
    //         std::cout << "Trigger Next Nomnation!" << "\n";
    //         lastPrepareTime = time(0);
    //     }
    // }
    std::cout << "Trigger Next Nomination!" << "\n";
    StellarConsensus2::GetInstance()->StartNominationProtocol();
    isReadyNow = false;
} 
