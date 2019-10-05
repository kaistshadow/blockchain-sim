#ifndef BL_MAIN_EVENT_MANAGER_H
#define BL_MAIN_EVENT_MANAGER_H

#include <ev++.h>
#include <queue>
#include <memory>

namespace libBLEEP_BL {

    enum class AsyncEventEnum {
        Base,
        Layer1_Event_Start,
        SocketAccept,
        SocketConnect,
        SocketConnectFailed,
        SocketRecv,
        SocketWrite,
        Layer1_Event_End,


        Layer2_Event_Start,
        CompleteAsyncConnectPeer,
        ErrorAsyncConnectPeer,
        NewPeerConnected,  /* connection estabilished by neighbor peer */
        PeerDisconnected,  /* connection disconnected by remote peer */
        Layer2_Event_End,


        Layer5_Event_END,
        AllEvent,
    };

    /* class for data */
    /* It needs a refactoring! Use polymorphism to reduce data overhead! */
    class AsyncEventDataManager {
    private:
        // data for SocketAccept event
        int _newlyAcceptedSocket;

        // data for SocketConnect event
        int _newlyConnectedSocket;

        // data for SocketConnectFailed event
        int _failedSocket;
        std::string _failedDomain;

        // data for SocketRecv event
        int _recvSocket;

        // data for SocketWrite event
        int _writeSocket;

    public:
        // data set function for SocketAccept
        void SetNewlyAcceptedSocket(int fd) { _newlyAcceptedSocket = fd; }
        // data access function for SocketAccept
        int GetNewlyAcceptedSocket() { return _newlyAcceptedSocket; }

        // data set function for SocketConnect
        void SetNewlyConnectedSocket(int fd) { _newlyConnectedSocket = fd; }
        // data access function for SocketConnect
        int GetNewlyConnectedSocket() { return _newlyConnectedSocket; }

        // data set function for SocketConnectFailed
        void SetFailedSocket(int fd) { _failedSocket = fd; }
        void SetFailedDomain(std::string s) { _failedDomain = s; }
        // data access function for SocketConnectFailed
        int GetFailedSocket() { return _failedSocket; }
        std::string GetFailedDomain() { return _failedDomain; }

        // data set function for SocketRecv
        void SetRecvSocket(int fd) { _recvSocket = fd; }
        // data access function for SocketRecv
        int GetRecvSocket() { return _recvSocket; }

        // data set function for SocketWrite
        void SetWriteSocket(int fd) { _writeSocket = fd; }
        // data access function for SocketWrite
        int GetWriteSocket() { return _writeSocket; }

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

        /* Internal data structures
           for managing the triggered asynchronous event */
        std::queue<AsyncEvent> _eventQueue;        

        /* Internally handling async events whose value is less than _intervalHanldeEventEnum */
        AsyncEventEnum _internalHandleEventEnum;



    public:
        /*********************************************************/
        /* Public API designed for main event loop controls */
        /*********************************************************/
        MainEventManager(AsyncEventEnum internalEventEnum = AsyncEventEnum::Base);

        /* blocking API that awaits for next asynchronous event */
        void Wait();

        void PushAsyncEvent(AsyncEvent event);

        bool ExistAsyncEvent() { return !_eventQueue.empty(); }
        AsyncEvent PopAsyncEvent();
    };

    extern std::unique_ptr<MainEventManager> g_mainEventManager;

}

#endif
