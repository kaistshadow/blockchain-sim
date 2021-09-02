//
// Created by Hyunjin Kim on 2021/06/04.
//

#include "shadow_interface.h"

#include "POSMiner.h"

using namespace libBLEEP_BL;

void POSMiner::AsyncMakeBlock(std::shared_ptr<POSBlock> blk) {
    double timestamp = libBLEEP::GetGlobalClock();
    blk->SetTimestamp(timestamp);
    blk->CalcHash();

    // push asynchronous event
    AsyncEvent event(AsyncEventEnum::EmuBlockMiningComplete);
    event.GetData().SetMinedPOSBlock(blk);
    MainEventManager::Instance()->PushAsyncEvent(event);
}