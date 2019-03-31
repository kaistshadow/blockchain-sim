#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <ev++.h>

#include "../datamanagermodules/ShadowPipeManager.h"
#include "../datamanagermodules/PipeManager.h"
#include "../datamanagermodules/SocketManager.h"
#include "../datamanagermodules/PeerManager.h"

#include "../datamodules/Peer.h"
#include "../datamodules/Transaction.h"

namespace libBLEEP {

    enum class AsyncEventEnum {
        none,
        CompleteAsyncConnectPeer,
        ErrorAsyncConnectPeer,
        CompleteAsyncGenerateRandomTransaction,
        RecvMessage,
    };

    class AsyncEventDataManager {
    private:
        // data for CompleteAsyncConnectPeer event
        PeerId _connectedPeerId;
        int _connectedSocketFD;

        // data for ErrorAsyncConnectPeer event
        PeerId _refusedPeerId;
        int _error;

        // data for CompleteAsyncGenerateRandomTransaction
        boost::shared_ptr<Transaction> _generatedTx;

        // data for RecvMessage
        std::shared_ptr<Message> _receivedMsg;
        
    public:
        // data set function for CompleteAsyncConnectPeer
        void SetConnectedPeerId(PeerId id) { _connectedPeerId = id; }
        void SetConnectedSocketFD(int fd) { _connectedSocketFD = fd; }
        // data access function for CompleteAsyncConnectPeer
        PeerId GetConnectedPeerId() { return _connectedPeerId; }
        int GetConnectedSocketFD() { return _connectedSocketFD; }

        // data set function for ErrorAsyncConnectPeer
        void SetRefusedPeerId(PeerId id) { _refusedPeerId = id; }
        void SetError(int err) { _error = err; }
        // data access function for ErrorAsyncConnectPeer
        PeerId GetRefusedPeerId() { return _refusedPeerId; }
        int GetError() { return _error; }

        // data set function for CompleteAsyncGenerateRandomTransaction
        void SetGeneratedTx(boost::shared_ptr<Transaction> tx) { _generatedTx = tx; }
        // data access function for CompleteAsyncGenerateRandomTransaction
        boost::shared_ptr<Transaction> GetGeneratedTx() { return _generatedTx; }

        // data set function for CompleteAsyncGenerateRandomTransaction
        void SetReceivedMsg(std::shared_ptr<Message> msg) { _receivedMsg = msg; }
        // data access function for CompleteAsyncGenerateRandomTransaction
        std::shared_ptr<Message> GetReceivedMsg() { return _receivedMsg; }
    };

    class MainEventManager {
    private:
        class AsyncConnectTimer {
        private:
            ev::timer _timer; // destructor automatically stops the watcher
            PeerId _id;
            MainEventManager* _eventManager;
            void _timerCallback(ev::timer &w, int revents) {
                std::cout << "AsyncConnect timer callback executes!" << "\n";
                _eventManager->AsyncConnectPeer(_id);
                delete this;
            }
        public:
            AsyncConnectTimer(PeerId id, double time, MainEventManager* manager) {
                _id = id;
                _eventManager = manager;
                _timer.set<AsyncConnectTimer, &AsyncConnectTimer::_timerCallback> (this);
                _timer.set(time, 0.);
                _timer.start();
                std::cout << "timer started!" << "\n";
            }
        };

        friend class AsyncGenerateRandomTransactionTimer;
        class AsyncGenerateRandomTransactionTimer {
        private:
            ev::timer _timer; // destructor automatically stops the watcher
            MainEventManager* _eventManager;
            void _timerCallback(ev::timer &w, int revents) {
                std::cout << "AsyncGenerateRandomTransaction timer callback executes!" << "\n";
                srand((unsigned int)time(0));
                int sender_id = rand() % 100;
                int receiver_id = rand() % 100;
                float amount = (float) (rand() % 10000);
                boost::shared_ptr<Transaction> generatedTx(new SimpleTransaction(sender_id, receiver_id, amount));
                _eventManager->_asyncEventTriggered = true;
                _eventManager->_nextAsyncEvent = AsyncEventEnum::CompleteAsyncGenerateRandomTransaction;
                _eventManager->_dataManager.SetGeneratedTx(generatedTx);
                delete this;
            }
        public:
            AsyncGenerateRandomTransactionTimer(double time, MainEventManager* manager) {
                _eventManager = manager;
                _timer.set<AsyncGenerateRandomTransactionTimer, &AsyncGenerateRandomTransactionTimer::_timerCallback> (this);
                _timer.set(time, 0.);
                _timer.start();
                std::cout << "timer started!" << "\n";
            }
        };

    private:
        /* internal event loop variable */
        struct ev_loop *_libev_loop;
    private:
        /*********************************************************/
        /* Internal data structures
           for managing the triggered asynchronous event */
        /*********************************************************/

        /* Event trigger status */
        bool _asyncEventTriggered = false;
        /* Type of triggered asynchronous event */
        AsyncEventEnum _nextAsyncEvent;
        /* Data manager for the triggered asynchronous event  */
        AsyncEventDataManager _dataManager;

        
    public:
        /*********************************************************/
        /* Public API designed for main event loop controls */
        /*********************************************************/

        /* Constructor with proper peer id */
        MainEventManager(std::string myPeerId);

        /* blocking API that awaits for next asynchronous event */
        void Wait();

        /* returning asynchronous event type */
        AsyncEventEnum GetEventType() { return _nextAsyncEvent; }
        
        /* returning helper module for asynchronous event data management */
        AsyncEventDataManager& GetAsyncEventDataManager() { return _dataManager; }        

    public:
        /*********************************************************/
        /* Public API designed for high-level event requests */
        /*********************************************************/

        /* asynchronous API that requests a connection for given peer */
        /* When the task is complete, 'complete event' will be triggered. */
        /* 'complete event' offers connected socket fd and requested PeerId */
        /* If the valid socket connection exists for given PeerId, 
           the function immediately returns false, and 'complete event' will not be triggered */
        /* If the 'time' is given, the API tries connection
           after the given 'time' is passed. */
        bool AsyncConnectPeer(PeerId id, double time = 0);

        /* asynchronous API that sends a given message to proper peer(s) */
        /* If the valid socket connection does not exist for given PeerId, 
           the function immediately returns false */
        /* There's no separate complete event for this API */
        bool UnicastMessage(PeerId dest, std::shared_ptr<Message> message);

        /* asynchronous API that requests a random generated transaction */
        /* Argument 'time' specifies the waiting time. 
           If the 'time' is greater than 0, the transaction will be generated 
           after the given 'time' is passed. */
        /* When the task is complete, 'complete event' will be triggered. */
        /* 'complete event' offers randomly generate Transaction. */
        void AsyncGenerateRandomTransaction(double time);


    private:
        /*********************************************************/
        /* Internal data structures
           for managing the asynchronous function call data.
           Current list of supported asynchronous function calls : 
             1) AsyncConnectPeer (supported at 20190328)
             2) AsyncGenerateRandomTransaction (supported at 20190331) */
        /*********************************************************/
        
        // for managing AsyncConnectPeer's requested data
        std::list<std::pair< PeerId, int > > _asyncConnectPeerRequests;


    private:
        // Internal management module for managing peers
        // It maintains the table of the all known peers having valid socket connection
        PeerManager peerManager;

    private:
        /*********************************************************/
        /* Internal management modules
           for managing the asynchronous IO events */
        /*********************************************************/
        
        /*************** for shadow-IO event handling ******************/
        // for managing shadow pipe structures
        ShadowPipeManager shadowPipeManager;

        /*************** for socket-IO event handling ******************/
        // for listening socket management
        ListenSocketManager listenSocketManager;
        // for connecting socket management
        ConnectSocketManager connectSocketManager;
        // for data socket management
        DataSocketManager dataSocketManager;

    private:
        /*********************************************************/
        /* Internal management modules
           for managing the asynchronous timer events */
        /*********************************************************/
        
        /* internal API that registers event for later execution */
        /* void ScheduleEvent(AsyncEventEnum eventType, double time); */


    private:
        void _HandleShadowEvents();
        void _HandleNetworkEvents();
        void _HandleTimerEvents();

    };


}




#endif
