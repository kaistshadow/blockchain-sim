#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include "../datamodules/Socket.h"

#include <map>
#include <memory>
#include <vector>

namespace libBLEEP {
    /* By giving multiple managers for each socket type, i.e., listenSocketManager, connectSocketManager,
       we can split the socket IO callback function for each socket type.
       This design choice is come from the limitation of libev such that, in the context of IO callback, 
       it is unable to know the type of event-triggered socket. 
       (Since, it is hard to give additional arguments to callback function) */
    class ListenSocketManager {
    private:
        std::map<int, std::shared_ptr<ListenSocket> > _sockets;  // map fd -> ListenSocket structure

        /* event io callback */
        void _listenSocketIOCallback (ev::io &w, int revents);
    
        /* event watcher */ /* maybe we should use map data structure? currently no way to remove watcher. */
        std::list<ev::io> _listenSocketWatchers;

        /* libev event management */
        bool _eventTriggered = false;
        int _eventTriggeredFD;
        SocketEventEnum _eventType;
    
        /* event set method */
        void _SetEventTriggered(bool triggered) { _eventTriggered = triggered; }
        void _SetEventTriggeredFD(int fd) { _eventTriggeredFD = fd; }
        void _SetEventType(SocketEventEnum type) { _eventType = type; }

    public:
        // create listening socket + create IO watcher for it
        int CreateListenSocket(int port = DEFAULT_SOCKET_PORT);

        // should we need method for deleting socket?

        /* get ListenSocket method */
        std::shared_ptr<ListenSocket> GetListenSocket(int fd);

        /* event get method */
        bool IsEventTriggered() { return _eventTriggered; }
        int GetEventTriggeredFD() { return _eventTriggeredFD; }
        SocketEventEnum GetEventType() { return _eventType; }

        /* event clear method */
        void ClearEventTriggered() { _eventTriggered = false; }
    };

    class ConnectSocketManager {
    private:
        std::map<int, std::shared_ptr<ConnectSocket> > _sockets;  // map fd -> ConnectSocket structure

        /* event io callback */
        void _connectSocketIOCallback (ev::io &w, int revents);
    
        /* event watcher */ 
        std::map<int, ev::io> _connectSocketWatchers;

        /* libev event management */
        bool _eventTriggered = false;
        int _eventTriggeredFD;
        SocketEventEnum _eventType;
    
        /* event set method */
        void _SetEventTriggered(bool triggered) { _eventTriggered = triggered; }
        void _SetEventTriggeredFD(int fd) { _eventTriggeredFD = fd; }
        void _SetEventType(SocketEventEnum type) { _eventType = type; }

    public:
        // create connecting socket + create IO watcher for it
        int CreateNonblockConnectSocket(std::string destination);

        // remove connecting socket and related IO watcher
        void RemoveConnectSocket(int fd);
        

        /* event get method */
        bool IsEventTriggered() { return _eventTriggered; }
        int GetEventTriggeredFD() { return _eventTriggeredFD; }
        SocketEventEnum GetEventType() { return _eventType; }

        /* event clear method */
        void ClearEventTriggered() { _eventTriggered = false; }
    };

    class DataSocketManager {
    private:
        std::map<int, std::shared_ptr<DataSocket> > _sockets;  // map fd -> ConnectSocket structure

        /* event io callback */
        void _dataSocketIOCallback (ev::io &w, int revents);
        
        /* event watcher */
        std::map<int, ev::io> _dataSocketWatchers;

        /* libev event management */
        bool _eventTriggered = false;
        int _eventTriggeredFD;
        SocketEventEnum _eventType;
    
        /* event set method */
        void _SetEventTriggered(bool triggered) { _eventTriggered = triggered; }
        void _SetEventTriggeredFD(int fd) { _eventTriggeredFD = fd; }
        void _SetEventType(SocketEventEnum type) { _eventType = type; }

    public:
        // When the CreateDataSocket is called?
        // when receiveConnection event is triggered : accept & create data socket
        // when socketConnected event is triggered : replace connect socket as data socket
        void CreateDataSocket(int sfd);

        // remove data socket from container and remove related IO watcher
        void RemoveDataSocket(int fd);

        /* get DataSocket method */
        std::shared_ptr<DataSocket> GetDataSocket(int fd);
        std::vector<std::shared_ptr<DataSocket> > GetAllDataSockets();

        /* set/unset writable */
        void SetWritable(int sfd);
        void UnsetWritable(int sfd);


        /* event get method */
        bool IsEventTriggered() { return _eventTriggered; }
        int GetEventTriggeredFD() { return _eventTriggeredFD; }
        SocketEventEnum GetEventType() { return _eventType; }
        
        /* event clear method */
        void ClearEventTriggered() { _eventTriggered = false; }

    };

}


#endif
