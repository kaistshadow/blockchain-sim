#ifndef POW_MODULE_H
#define POW_MODULE_H

#include "MainEventManager.h"
#include "../datamodules/POWBlock.h"
#include "../crypto/SHA256.h"
#include "../utility/GlobalClock.h"
#include "../utility/Assert.h"
#include <thread>
#include <atomic>

#include "shadow_interface.h"

namespace libBLEEP {

    class POWModule {
    private:
        class BlockMiningEmulationTimer; // forward declaration for nested class
        class BlockMiningThread; // forward declaration for nested class

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

            /* miningTimer watcher */
            std::shared_ptr<BlockMiningEmulationTimer> _miningTimerWatcher;

            /* miningThread watcher */
            std::shared_ptr<BlockMiningThread> _miningThreadWatcher;

        public:
            WatcherManager(POWModule* powModule, MainEventManager* eventModule)
                : _powModule(powModule), _mainEventModule(eventModule) {};
            void CreateMiningEmulationTimer(std::shared_ptr<POWBlock> candidateBlk, double time) {
                M_Assert(_miningTimerWatcher == nullptr, "there exists miningTimerWatcher already");
                // allocate new MiningEmulationTimer 
                _miningTimerWatcher = std::make_shared<BlockMiningEmulationTimer>(candidateBlk, time, _powModule, _mainEventModule);
            }

            void RemoveMiningEmulationTimer() {
                _miningTimerWatcher = nullptr; // it will automatically destroy BlockMiningEmulationTimer object
            }

            void CreateMiningThread(std::shared_ptr<POWBlock> candidateBlk, UINT256_t difficulty) {
                M_Assert(_miningThreadWatcher == nullptr, "there exists miningThreadWatcher already");
                // allocate new MiningThread
                _miningThreadWatcher = std::make_shared<BlockMiningThread>(candidateBlk, difficulty, _powModule, _mainEventModule);
            }

            void StopMiningThread() {
                if (_miningThreadWatcher) {
                    _miningThreadWatcher->Stop();
                }
            }
            void RemoveMiningThread() {
                _miningThreadWatcher = nullptr; // it will automatically destroy BlockMiningThread object
            }
        };


        class BlockMiningEmulationTimer : public WatcherWrapper {
        private:
            ev::timer _timer; // destructor automatically stops the watcher
            std::shared_ptr<const POWBlock> _candidateBlk;

            void _timerCallback(ev::timer &w, int revents) {
                /* init_shadow_clock_update();                 */
                PrintTimespec("POW miningTimer callback called");

                // 1. calculate random block.
                srand((unsigned int)time(0));
                /* unsigned long nonce = 0;  // since this is an emulator, we use arbitrary nonce value. */
                unsigned long nonce = _powModule->GetHostNumber();  // use arbitrary random value for emulation. (hardcoded )
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
                PrintTimespec("POW miningTimer callback ended");
                /* next_shadow_clock_update("==== done handling miningTimer callback"); */
            }
        public:
            BlockMiningEmulationTimer(std::shared_ptr<POWBlock> candidateBlk, double time, POWModule* powModule, MainEventManager* eventModule)
                : WatcherWrapper (powModule, eventModule) {
                _candidateBlk = candidateBlk;
                _timer.set<BlockMiningEmulationTimer, &BlockMiningEmulationTimer::_timerCallback>(this);
                _timer.set(time, 0.);
                _timer.start();
                /* std::cout << "block mining timer started!" << "\n"; */
                _powModule->_isMining = true;
            }
            ~BlockMiningEmulationTimer() {
                /* std::cout << "BlockMiningEmulationTimer destroyed" << "\n"; */
            }
        };


        class BlockMiningThread : public WatcherWrapper {
        private:
            ev::async _async_watcher; // destructor automatically stops the watcher
            std::shared_ptr<const POWBlock> _candidateBlk;
            std::shared_ptr<POWBlock> _validBlk;
            UINT256_t _difficulty;
            int _trial = 1000; // number of trials before cooperatively yield the thread

            std::thread _thread;
            std::atomic<bool> _thread_exit;

            // for printing time
            struct timespec start, end;

            void _run() {
                // thread for mining
                srand((unsigned int)time(0));                    
                while (true) {
                    /* clock_gettime(CLOCK_MONOTONIC, &start); */
                    for (int i = 0; i < _trial; i++) {
                        unsigned long nonce = rand() * rand();
                        unsigned char hash_out[32];

                        unsigned long blockidx = _candidateBlk->GetBlockIdx();
                        SHA256_CTX ctx;
                        sha256_init(&ctx);
                        sha256_update(&ctx, (const unsigned char*)&nonce, sizeof(unsigned long));
                        sha256_update(&ctx, (const unsigned char*)_candidateBlk->GetTxHash().str().c_str(), _candidateBlk->GetTxHash().str().size());
                        sha256_update(&ctx, (const unsigned char*)&blockidx, sizeof(unsigned long));
                        sha256_update(&ctx, (const unsigned char*)_candidateBlk->GetPrevBlockHash().str().c_str(), _candidateBlk->GetPrevBlockHash().str().size());
                        double timestamp = GetGlobalClock();
                        sha256_update(&ctx, (const unsigned char*)&timestamp, sizeof(double));
                        sha256_final(&ctx, hash_out);

                        UINT256_t hash_out_256(hash_out, 32);
                        if (hash_out_256 < _difficulty) {
                            // make a copy of candidate block to produce valid block
                            _validBlk = std::make_shared<POWBlock>(*_candidateBlk);
                            _validBlk->SetNonce(nonce);
                            _validBlk->SetBlockHash(hash_out_256);
                            _validBlk->SetTimestamp(timestamp);
                            _validBlk->SetDifficulty(_difficulty);
                        
                            // Terminate current thread.
                            // before thread termination
                            // notify the watcherWrapper for the delegation of the destruction
                            _async_watcher.send();
                            return;
                        }
                    }
                    /* clock_gettime(CLOCK_MONOTONIC, &end); */
                    /* PrintTimeDiff("time for executing thread loop.", start, end); */
                    // The value printed above will be leveraged for approximate sleep time in below.

                    /* approximate time(in us) for executing above loop one time
                       The value '5850' is obtained from above PrintTimeDiff in my local machine
                       ,i.e., i7-6700 3.4GHz machine with 16GM mem. */
                    /* shadow_usleep(5850);   */
                    if (_thread_exit) {
                        // Check the atomic flag, and if it's set, terminate current thread.
                        std::cout << "thread exits!" << "\n";
                        return;
                    }

                } // run forever until finding valid block
            }

            void _asyncCallback(ev::async &w, int revents) {
                std::cout << "check for thread termination" << "\n";
                _thread.join();
                std::cout << "thread is safely terminated" << "\n";
                
                // push asynchronous event
                AsyncEvent event(AsyncEventEnum::BlockMiningComplete);
                event.GetData().SetMinedBlock(_validBlk);
                _mainEventModule->PushAsyncEvent(event);

                _powModule->_isMining = false;
                _powModule->watcherManager.RemoveMiningThread();
                // it will eventually remove myself (BlockMiningThread object);
            }

        public:
            BlockMiningThread(std::shared_ptr<POWBlock> candidateBlk, 
                              UINT256_t difficulty, 
                              POWModule* powModule, MainEventManager* eventModule)
                : WatcherWrapper (powModule, eventModule) {
                _candidateBlk = candidateBlk;
                _difficulty = difficulty;
                _async_watcher.set<BlockMiningThread, &BlockMiningThread::_asyncCallback>(this);
                _async_watcher.start();

                _thread_exit = false;
                std::cout << "block mining thread before!" << "\n";                
                _thread = std::thread (&BlockMiningThread::_run, this);
                std::cout << "block mining thread started!" << "\n";
                _powModule->_isMining = true;
            }
            ~BlockMiningThread() {
                std::cout << "BlockMiningThread destroyed" << "\n";
            }

            void Stop() {
                _powModule->_isMining = false;

                _thread_exit = true;
                _thread.join(); // wait until the child thread terminates 
                _powModule->watcherManager.RemoveMiningThread();
                // it will eventually remove myself (BlockMiningThread object)
            }
        };

    private:
        std::string _myPeerId;
        MainEventManager* _mainEventManager;

    private:
        WatcherManager watcherManager;
        // internal data structure
        bool _isMining = false;

    public:
        /* Constructor */
        POWModule(MainEventManager* eventManager) 
            : _mainEventManager(eventManager), watcherManager(this, eventManager) {};

        /* Constructor */
    POWModule(std::string myPeerId, MainEventManager* eventManager) 
            : _myPeerId(myPeerId), _mainEventManager(eventManager), watcherManager(this, eventManager) {};

        int GetHostNumber() {
            if (_myPeerId.find("bleep") == 0) {
                int result = std::stoi(_myPeerId.substr(5, _myPeerId.size()));
                std::cout << "nodeid contains 'bleep'. hostnumber:" << result << "\n";

                return result;
            }
            // assume ip is given as id. 
            // TODO : support for arbitrary id.
            else {
                unsigned int ips[4];
                sscanf(_myPeerId.c_str(), "%u.%u.%u.%u", &ips[3], &ips[2], &ips[1], &ips[0]);
                unsigned int val = ips[0] + ips[1]*256 + ips[2]*256*256 + ips[3]*256*256*256;
                return val;
                /* std::cout << "nodeid does not contain 'bleep'. _myPeerId:" << _myPeerId << "\n"; */
            }
        }

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

        /* asynchronous API that requests a block mining 
           Argument 'candidateBlk' specifies the candidate block for mining
           Argument 'difficulty' specifies the 256bit threshold value for valid block.
            For example, if difficulty is '0x0000.....00ffff', 
            the hash value 'H' of the newly generated block should be less than 
            the difficulty value, i.e., H <= '0x0000....00ffff'.
           When the mining task is complete, 'complete event'
           called 'BlockMiningComlete' will be triggered. 
           The 'complete event' offers generated valid block. */
        void AsyncBlockMining(std::shared_ptr<POWBlock> candidateBlk, UINT256_t difficulty);

        bool IsMining() { return _isMining; }

        void StopMining();
            
    };





}



#endif
