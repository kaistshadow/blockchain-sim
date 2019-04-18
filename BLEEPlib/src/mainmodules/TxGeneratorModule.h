#ifndef TX_GENERATOR_MODULE_H
#define TX_GENERATOR_MODULE_H

#include "MainEventManager.h"

namespace libBLEEP {

    class TxGeneratorModule {
    private:
        class WatcherWrapper {
        protected:
            MainEventManager* _mainEventModule;
        public:
            WatcherWrapper(MainEventManager* eventModule)
                : _mainEventModule(eventModule) {};
        };

        class AsyncGenerateRandomTransactionTimer : public WatcherWrapper {
        private:
            ev::timer _timer; // destructor automatically stops the watcher
            void _timerCallback(ev::timer &w, int revents) {
                std::cout << "AsyncGenerateRandomTransaction timer callback executes!" << "\n";
                srand((unsigned int)time(0));
                int sender_id = rand() % 100;
                int receiver_id = rand() % 100;
                float amount = (float) (rand() % 10000);
                boost::shared_ptr<Transaction> generatedTx(new SimpleTransaction(sender_id, receiver_id, amount));

                // push asynchronous event
                AsyncEvent event(AsyncEventEnum::CompleteAsyncGenerateRandomTransaction);
                event.GetData().SetGeneratedTx(generatedTx);
                _mainEventModule->PushAsyncEvent(event);
                delete this;
            }
        public:
            AsyncGenerateRandomTransactionTimer(double time, MainEventManager* eventModule)
                : WatcherWrapper(eventModule) {
                _timer.set<AsyncGenerateRandomTransactionTimer, &AsyncGenerateRandomTransactionTimer::_timerCallback> (this);
                _timer.set(time, 0.);
                _timer.start();
                std::cout << "timer started!" << "\n";
            }
        };

    private:
        MainEventManager* _mainEventManager;

    public:
        /* Constructor with proper peer id */
        TxGeneratorModule(MainEventManager* eventManager)
            : _mainEventManager(eventManager) {}

        /*********************************************************/
        /* Public API designed for high-level event requests */
        /*********************************************************/

        /* asynchronous API that requests a random generated transaction */
        /* Argument 'time' specifies the waiting time. 
           If the 'time' is greater than 0, the transaction will be generated 
           after the given 'time' is passed. */
        /* When the task is complete, 'complete event' will be triggered. */
        /* 'complete event' offers randomly generate Transaction. */
        void AsyncGenerateRandomTransaction(double time);
    };

}

#endif


