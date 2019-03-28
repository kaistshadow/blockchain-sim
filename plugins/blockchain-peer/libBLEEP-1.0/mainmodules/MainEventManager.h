#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <ev++.h>

#include "../datamanagermodules/ShadowPipeManager.h"
#include "../datamanagermodules/PipeManager.h"
#include "../datamanagermodules/SocketManager.h"
#include "../datamanagermodules/PeerManager.h"

#include "../datamodules/Peer.h"

namespace libBLEEP {

    enum class AsyncEventEnum {
        none,
        CompleteAsyncConnectPeer,

    };

    class AsyncEventDataManager {
    private:
        // data for CompleteAsyncConnectPeer event
        PeerId _connectedPeerId;
        int _connectedSocketFD;
        
    public:
        // data set function for CompleteAsyncConnectPeer
        void SetConnectedPeerId(PeerId id) { _connectedPeerId = id; }
        void SetConnectedSocketFD(int fd) { _connectedSocketFD = fd; }
        // data access function for CompleteAsyncConnectPeer
        PeerId GetConnectedPeerId() { return _connectedPeerId; }
        int GetConnectedSocketFD() { return _connectedSocketFD; }
    };

    class MainEventManager {
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
        bool AsyncConnectPeer(PeerId id);

        /* asynchronous API that sends a given message to proper peer(s) */
        void SendMessage(Message message);

        /* asynchronous API that requests a random generated transaction 
           Argument 'time' specifies the waiting time. 
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
             2) AsyncGenerateRandomTransaction (supported at 20190328) */
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
