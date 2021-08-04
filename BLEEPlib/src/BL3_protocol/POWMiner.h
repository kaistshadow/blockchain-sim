// "Copyright [2021] <kaistshadow>"

//
// Created by Yonggon Kim on 2021/04/25.
//

#ifndef BLEEPLIB_SRC_BL3_PROTOCOL_POWMINER_H_
#define BLEEPLIB_SRC_BL3_PROTOCOL_POWMINER_H_

#include <memory>
#include "../BL_MainEventManager.h"
#include "../utility/GlobalClock.h"
#include "../crypto/SHA256.h"
#include "POWBlock.h"
#include <ev++.h>

#include "shadow_interface.h"

namespace libBLEEP_BL {
class POWMiner {
 public:
    POWMiner() {}

    /*********************************************************/
    /* Public API designed for high-level event requests */
    /*********************************************************/

    /* asynchronous API that requests a block mining
        Argument 'candidateBlk' specifies the candidate block for mining
        Argument 'lambda' specifies (1/average time for completing mining)
        When the mining emulation task is complete, 'complete event'
        called 'EmuBlockMiningComlete' will be triggered.
        The 'complete event' offers generated valid block. */
    void AsyncEmulateBlockMining(std::shared_ptr<POWBlock> candidateBlk, double lambda);

    bool IsMining() { return _isMining; }
    void StopMining();

 private:
    ev::timer _timer;  //mining timer
    std::shared_ptr<const POWBlock> _candidateBlk;
    bool _isMining = false;

    void _timerCallback(ev::timer &w, int revents) {
        /* init_shadow_clock_update();                 */
        libBLEEP::PrintTimespec("POW miningTimer callback called");

        // 1. calculate random block.
        srand((unsigned int)time(0));

        unsigned int randvalue = rand();
        unsigned int random_num = shadow_assign_virtual_id() +randvalue;
        unsigned long nonce = random_num;
        unsigned char hash_out[32];

        unsigned long blockidx = _candidateBlk->GetBlockIdx();
        SHA256_CTX ctx;
        sha256_init(&ctx);
        sha256_update(&ctx, (const unsigned char*)&nonce, sizeof(unsigned long));
        sha256_update(&ctx, (const unsigned char*)_candidateBlk->GetTxHash().str().c_str(), _candidateBlk->GetTxHash().str().size());
        sha256_update(&ctx, (const unsigned char*)&blockidx, sizeof(unsigned long));
        sha256_update(&ctx, (const unsigned char*)_candidateBlk->GetPrevBlockHash().str().c_str(), _candidateBlk->GetPrevBlockHash().str().size());

        double timestamp = libBLEEP::GetGlobalClock();
        sha256_update(&ctx, (const unsigned char*)&timestamp, sizeof(double));
        sha256_final(&ctx, hash_out);

        // make a copy of candidate block to produce valid block (successfully mined block)
        std::shared_ptr<POWBlock> validBlk = std::make_shared<POWBlock>(*_candidateBlk);
        validBlk->SetNonce(nonce);
        libBLEEP::UINT256_t hash_out_256(hash_out, 32);
        validBlk->SetBlockHash(hash_out_256);
        validBlk->SetTimestamp(timestamp);

        // push asynchronous event
        AsyncEvent event(AsyncEventEnum::EmuBlockMiningComplete);
        event.GetData().SetMinedBlock(validBlk);
        MainEventManager::Instance()->PushAsyncEvent(event);

        _isMining = false;
        libBLEEP::PrintTimespec("POW miningTimer callback ended");
        /* next_shadow_clock_update("==== done handling miningTimer callback"); */
    }
};
}  // namespace libBLEEP_BL

#endif  // BLEEPLIB_SRC_BL3_PROTOCOL_POWMINER_H_
