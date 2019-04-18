#ifndef MAIN_EVENT_MANAGER_H
#define MAIN_EVENT_MANAGER_H

#include <ev++.h>
#include <queue>

#include "../datamodules/Transaction.h"
#include "../datamodules/Peer.h"
#include "../datamodules/Message.h"
#include "../datamodules/POWBlock.h"

namespace libBLEEP {

    enum class AsyncEventEnum {
        none,
        CompleteAsyncConnectPeer,
        ErrorAsyncConnectPeer,
        CompleteAsyncGenerateRandomTransaction,
        RecvMessage,
        NewPeerConnected,  /* connection estabilished by non-requested neighbor peer */
        PeerDisconnected,  /* connection disconnected by remote peer */
        EmuBlockMiningComplete, /* emulated mining for user-requested block is complete */
    };

    /* class for data */
    /* It needs a refactoring! Use polymorphism to reduce data overhead! */
    class AsyncEventDataManager {
    private:
        // data for CompleteAsyncConnectPeer event
        PeerId _connectedPeerId;
        int _connectedSocketFD;

        // data for ErrorAsyncConnectPeer event
        PeerId _refusedPeerId;
        int _error;
        std::string _errorMsg;

        // data for CompleteAsyncGenerateRandomTransaction
        boost::shared_ptr<Transaction> _generatedTx;

        // data for RecvMessage
        std::shared_ptr<Message> _receivedMsg;

        // data for NewPeerConnected 
        std::shared_ptr<PeerId> _newConnectedPeerId;

        // data for PeerDisconnected
        std::shared_ptr<PeerId> _disconnectedPeerId;

        // data for EmuBlockMiningComplete
        std::shared_ptr<POWBlock> _minedBlk;

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
        void SetErrorMsg(std::string errMsg) { _errorMsg = errMsg; }
        // data access function for ErrorAsyncConnectPeer
        PeerId GetRefusedPeerId() { return _refusedPeerId; }
        int GetError() { return _error; }
        std::string GetErrorMsg() { return _errorMsg; }

        // data set function for CompleteAsyncGenerateRandomTransaction
        void SetGeneratedTx(boost::shared_ptr<Transaction> tx) { _generatedTx = tx; }
        // data access function for CompleteAsyncGenerateRandomTransaction
        boost::shared_ptr<Transaction> GetGeneratedTx() { return _generatedTx; }

        // data set function for RecvMessage
        void SetReceivedMsg(std::shared_ptr<Message> msg) { _receivedMsg = msg; }
        // data access function for RecvMessage
        std::shared_ptr<Message> GetReceivedMsg() { return _receivedMsg; }

        // data set function for NewPeerConnected
        void SetNewlyConnectedPeer(std::shared_ptr<PeerId> id) { _newConnectedPeerId = id; }
        // data access function for NewPeerConnected
        std::shared_ptr<PeerId> GetNewlyConnectedPeer() { return _newConnectedPeerId; }

        // data set function for PeerDisconnected
        void SetDisconnectedPeer(std::shared_ptr<PeerId> id) { _disconnectedPeerId = id; }
        // data access function for PeerDisconnected
        std::shared_ptr<PeerId> GetDisconnectedPeerId() { return _disconnectedPeerId; }

        // data set function for EmuBlockMiningComplete
        void SetMinedBlock(std::shared_ptr<POWBlock> block) { _minedBlk = block; }
        // data access function for EmuBlockMiningComplete
        std::shared_ptr<POWBlock> GetMinedBlock() { return _minedBlk; }
    };

    class AsyncEvent {
    private:
        AsyncEventEnum _eventType;
        AsyncEventDataManager _eventData;
        
    public:
        AsyncEvent(AsyncEventEnum type)
            : _eventType(type) {}

        AsyncEventEnum GetType() { return _eventType; }
        AsyncEventDataManager& GetData() { return _eventData; }
    };

    class MainEventManager {
    private:
        /* internal event loop variable */
        struct ev_loop *_libev_loop;
    private:
        friend class BasicNetworkModule;
        friend class POWModule;
        /*********************************************************/
        /* Internal data structures
           for managing the triggered asynchronous event */
        /*********************************************************/

        /* /\* Event trigger status *\/ */
        /* bool _asyncEventTriggered = false; */
        /* /\* Type of triggered asynchronous event *\/ */
        /* AsyncEventEnum _nextAsyncEvent; */
        /* /\* Data manager for the triggered asynchronous event  *\/ */
        /* AsyncEventDataManager _dataManager; */

        std::queue<AsyncEvent> _eventQueue;

    public:
        /*********************************************************/
        /* Public API designed for main event loop controls */
        /*********************************************************/

        MainEventManager();
        /* blocking API that awaits for next asynchronous event */
        void Wait();

        void PushAsyncEvent(AsyncEvent event);

        bool ExistAsyncEvent() { return !_eventQueue.empty(); }
        AsyncEvent PopAsyncEvent();

        /* /\* returning asynchronous event type *\/ */
        /* AsyncEventEnum GetEventType() { return _nextAsyncEvent; } */
        
        /* /\* returning helper module for asynchronous event data management *\/ */
        /* AsyncEventDataManager& GetAsyncEventDataManager() { return _dataManager; }         */

    };

}








#endif
