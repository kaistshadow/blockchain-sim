#ifndef POW_MODULE_H
#define POW_MODULE_H

#include "MainEventManager.h"
#include "../datamodules/POWBlock.h"
#include "../crypto/SHA256.h"
#include "../utility/GlobalClock.h"

namespace libBLEEP {

    class POWModule {
    private:
        class BlockMiningEmulationTimer; // forward declaration for nested class

        class WatcherWrapper {
        protected:
            POWModule* _powModule;
            MainEventManager* _mainEventModule;
        public:
            WatcherWrapper(POWModule* powModule, MainEventManager* eventModule)
                : _powModule(powModule), _mainEventModule(eventModule) {};
        };

        class WatcherManager {
        private:
            POWModule* _powModule;
            MainEventManager* _mainEventModule;

        public:
            /* event watcher */
            std::shared_ptr<BlockMiningEmulationTimer> _miningTimerWatcher;

            WatcherManager(POWModule* powModule, MainEventManager* eventModule)
                : _powModule(powModule), _mainEventModule(eventModule) {};
            void CreateMiningEmulationTimer(std::shared_ptr<POWBlock> candidateBlk, double time) {
                // allocate new MiningEmulationTimer 
                _miningTimerWatcher = std::make_shared<BlockMiningEmulationTimer>(candidateBlk, time, _powModule, _mainEventModule);
            }

            void RemoveMiningEmulationTimer() {
                _miningTimerWatcher = nullptr; // it will automatically destroy BlockMiningEmulationTimer object
            }
        };


        class BlockMiningEmulationTimer : public WatcherWrapper {
        private:
            ev::timer _timer; // destructor automatically stops the watcher
            std::shared_ptr<const POWBlock> _candidateBlk;

            void _timerCallback(ev::timer &w, int revents) {
                // 1. calculate random block.
                srand((unsigned int)time(0));
                unsigned long nonce = 0;  // since this is an emulator, we use arbitrary nonce value.
                unsigned char hash_out[32];
                
                unsigned long blockidx = _candidateBlk->GetBlockIdx();
                SHA256_CTX ctx;
                sha256_init(&ctx);
                sha256_update(&ctx, (const unsigned char*)&nonce, sizeof(unsigned long));
                sha256_update(&ctx, (const unsigned char*)_candidateBlk->GetTxHash().str().c_str(), _candidateBlk->GetTxHash().str().size());
                sha256_update(&ctx, (const unsigned char*)&blockidx, sizeof(unsigned long));
                sha256_update(&ctx, (const unsigned char*)_candidateBlk->GetPrevBlockHash().str().c_str(), _candidateBlk->GetPrevBlockHash().str().size());
                /* double timestamp = utility::GetGlobalClock(); */
                double timestamp = GetGlobalClock();
                sha256_update(&ctx, (const unsigned char*)&timestamp, sizeof(double));
                sha256_final(&ctx, hash_out);

                // make a copy of candidate block to produce valid block (successfully mined block)
                std::shared_ptr<POWBlock> validBlk = std::make_shared<POWBlock>(*_candidateBlk);
                validBlk->SetNonce(nonce);
                UINT256_t hash_out_256(hash_out, 32);
                validBlk->SetBlockHash(hash_out_256);
                validBlk->SetTimestamp(timestamp);

                // push asynchronous event
                AsyncEvent event(AsyncEventEnum::EmuBlockMiningComplete);
                event.GetData().SetMinedBlock(validBlk);
                _mainEventModule->PushAsyncEvent(event);


                _powModule->_isMining = false;
                _powModule->watcherManager.RemoveMiningEmulationTimer();
                // it will eventually remove myself (BlockMiningEmulationTimer object)
            }
        public:
            BlockMiningEmulationTimer(std::shared_ptr<POWBlock> candidateBlk, double time, POWModule* powModule, MainEventManager* eventModule)
                : WatcherWrapper (powModule, eventModule) {
                _candidateBlk = candidateBlk;
                _timer.set<BlockMiningEmulationTimer, &BlockMiningEmulationTimer::_timerCallback>(this);
                _timer.set(time, 0.);
                _timer.start();
                std::cout << "block mining timer started!" << "\n";
                _powModule->_isMining = true;
            }
            ~BlockMiningEmulationTimer() {
                std::cout << "BlockMiningEmulationTimer destroyed" << "\n";
            }
        };


    private:
        MainEventManager* _mainEventManager;

    private:
        WatcherManager watcherManager;
        // internal data structure
        bool _isMining = false;

    public:
        /* Constructor */
        POWModule(MainEventManager* eventManager) 
            : _mainEventManager(eventManager), watcherManager(this, eventManager) {};

        /*********************************************************/
        /* Public API designed for high-level event requests */
        /*********************************************************/
            
        /* asynchronous API that requests a block mining */
        /* Argument 'candidateBlk' specifies the candidate block for mining */
        /* Argument 'avg' specifies average time for completing mining */
        /* Argument 'stddev' specifies standard deviation of the time for completing mining */
        /* When the mining emulation task is complete, 'complete event'
           called 'EmuBlockMiningComlete' will be triggered. */
        /* The 'complete event' offers generated valid block. */
        void AsyncEmulateBlockMining(std::shared_ptr<POWBlock> candidateBlk, double avg, double stddev);

        bool IsMining() { return _isMining; }

        void StopMining();
            
    };





}



#endif
