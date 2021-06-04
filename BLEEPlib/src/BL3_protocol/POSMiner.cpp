//
// Created by Hyunjin Kim on 2021/06/04.
//

#include "shadow_interface.h"

#include "POSMiner.h"

using namespace libBLEEP_BL;

void POSMiner::AsyncMakeBlock(std::shared_ptr<POSBLOCK> templateBlock) {
    double timestamp = libBLEEP::GetGlobalClock();
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (const unsigned char*)blk->GetTxHash().str().c_str(), blk->GetTxHash().str().size());
    sha256_update(&ctx, (const unsigned char*)blk->GetPrevBlockHash().str().c_str(), blk->GetPrevBlockHash().str().size());
    sha256_update(&ctx, (const unsigned char*)&slot_no, sizeof(unsigned int));
    sha256_update(&ctx, (const unsigned char*)&creator, sizeof(unsigned int));
    sha256_update(&ctx, (const unsigned char*)&timestamp, sizeof(double));
    sha256_final(&ctx, hash_out);

    // make a copy of candidate block to produce valid block (successfully mined block)
    std::shared_ptr<POSBlock> blk = std::make_shared<POSBlock>(*templateBlock);
    libBLEEP::UINT256_t hash_out_256(hash_out, 32);
    blk->SetBlockHash(hash_out_256);
    blk->SetTimestamp(timestamp);

    // push asynchronous event
    AsyncEvent event(AsyncEventEnum::EmuBlockMiningComplete);
    event.GetData().SetMinedBlock(blk);
    MainEventManager::Instance()->PushAsyncEvent(event);
}