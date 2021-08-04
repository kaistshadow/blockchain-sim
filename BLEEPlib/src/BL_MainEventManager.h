// "Copyright [2021] <kaistshadow>"
#ifndef BLEEPLIB_SRC_BL_MAINEVENTMANAGER_H_
#define BLEEPLIB_SRC_BL_MAINEVENTMANAGER_H_

#include <ev++.h>
#include <queue>
#include <memory>
#include <string>

#include "BL2_peer_connectivity/Peer.h"
#include "BL2_peer_connectivity/Message.h"
#include "BL3_protocol/POWBlock.h"

namespace libBLEEP_BL {

class DataSocket;

enum class AsyncEventEnum {
    Base,                       // 0
    Layer1_Event_Start,         // 1
    SocketAccept,               // 2
    SocketConnect,              // 3
    SocketConnectFailed,        // 4
    SocketRecv,                 // 5
    SocketWrite,                // 6
    Layer1_Event_End,


    Layer2_Event_Start,
    BitcoinRecvMsg,             // 9

    //
    PeerSocketConnect,          // 10
    PeerSocketConnectFailed,    // 11
    PeerSocketClose,            // 12
    PeerRecvMsg,                // 13
    PeerRecvNotifyPeerId,       // 14
    // CompleteAsyncConnectPeer,
    // ErrorAsyncConnectPeer,
    // NewPeerConnected,  /* connection estabilished by neighbor peer */
    // PeerDisconnected,  /* connection disconnected by remote peer */
    Layer2_Event_End,


    Layer3_Event_Start,
    ProtocolRecvMsg,            // 17
    EmuBlockMiningComplete,     // 18
    Layer3_Event_End,

    UnitTest_Event_Start,
    FinishTest,                 // 21
    UnitTest_Event_End,

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

    // data for BitcoinRecvMsg event
    int _bitcoinRecvSocket;
    std::string _bitcoinCommand;
    uint32_t _bitcoinPayloadLen;
    uint32_t _bitcoinPayloadChecksum;
    std::string _bitcoinPayload;

    // data for PeerSocketConnect event
    std::shared_ptr<DataSocket> _dataSocket;

    // data for PeerSocketClose event
    std::shared_ptr<DataSocket> _closedSocket;

    // data for PeerRecvNotifyPeerId event
    PeerId _neighborId;
    std::shared_ptr<DataSocket> _incomingSocket;

    // data for PeerRecvMsg event
    PeerId _sourceId;
    std::shared_ptr<Message> _message;

    // data for ProtocolRecvMsg event
    std::shared_ptr<Message> _protocolMsg;

    // data for EmuBlockMiningComplete
    std::shared_ptr<POWBlock> _minedBlk;

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

    // data set function for BitcoinRecvMsg
    void SetBitcoinRecvSocket(int sockfd) { _bitcoinRecvSocket = sockfd; }
    void SetBitcoinCommand(std::string cmd) { _bitcoinCommand = cmd; }
    void SetBitcoinPayload(std::string payload) { _bitcoinPayload = payload; }
    void SetBitcoinPayloadLen(uint32_t len) { _bitcoinPayloadLen = len; }
    void SetBitcoinPayloadChecksum(uint32_t checksum) { _bitcoinPayloadChecksum = checksum; }
    // data access function for BitcoinRecvMsg
    int GetBitcoinRecvSocket() { return _bitcoinRecvSocket; }
    std::string GetBitcoinCommand() { return _bitcoinCommand; }
    std::string GetBitcoinPayload() { return _bitcoinPayload; }
    uint32_t GetBitcoinPayloadLen() { return _bitcoinPayloadLen; }
    uint32_t GetBitcoinPayloadChecksum() { return _bitcoinPayloadChecksum; }

    // data set function for PeerRecvNotifyPeerId
    void SetNeighborId(PeerId peerId) { _neighborId = peerId; }

    void SetIncomingSocket(std::shared_ptr<DataSocket> sock) { _incomingSocket = sock; }

    // data access function for PeerRecvNotifyPeerId
    PeerId GetNeighborId() { return _neighborId; }

    std::shared_ptr<DataSocket> GetIncomingSocket() { return _incomingSocket; }

    // data set function for PeerSocketConnect
    void SetDataSocket(std::shared_ptr<DataSocket> sock) { _dataSocket = sock; }

    // data access function for PeerSocketConnect
    std::shared_ptr<DataSocket> GetDataSocket() { return _dataSocket; }

    // data set function for PeerSocketConnectFailed
    // SetFailedDomain
    // data access function for PeerSocketConnectFailed
    // GetFailedDomain

    // data set function for PeerSocketClose
    void SetClosedSocket(std::shared_ptr<DataSocket> sock) { _closedSocket = sock; }

    // data access function for PeerSocketClose
    std::shared_ptr<DataSocket> GetClosedSocket() { return _closedSocket; }

    // data set function for PeerRecvNotifyPeerId
    void SetMsgSourceId(PeerId peerId) { _sourceId = peerId; }

    void SetMsg(std::shared_ptr<Message> msg) { _message = msg; }

    // data access function for PeerRecvNotifyPeerId
    PeerId GetMsgSourceId() { return _sourceId; }
    std::shared_ptr<Message> GetMsg() { return _message; }

    // data set function for ProtocolRecvMsg
    void SetProtocolMsg(std::shared_ptr<Message> msg) { _protocolMsg = msg; }
    // data access function for ProtocolRecvMsg
    std::shared_ptr<Message> GetProtocolMsg() { return _protocolMsg; }

    // data set function for EmuBlockMiningComplete, BlockMiningComplete
    void SetMinedBlock(std::shared_ptr<POWBlock> block) { _minedBlk = block; }
    // data access function for EmuBlockMiningComplete, BlockMiningComplete
    std::shared_ptr<POWBlock> GetMinedBlock() { return _minedBlk; }
};

class AsyncEvent {
 private:
    AsyncEventEnum _eventType;
    AsyncEventDataManager _eventData;

 public:
    explicit AsyncEvent(AsyncEventEnum type)
        : _eventType(type) {}

    AsyncEventEnum GetType() { return _eventType; }
    AsyncEventDataManager& GetData() { return _eventData; }
};

class MainEventManager {
 private:
    ev::timer _timeout;
    bool _timeout_triggered = false;
    void _timercallback(ev::timer& w, int revents) {
        _timeout_triggered = true;
    }
    void _startTimer(double timeout) {
        _timeout.set<MainEventManager, &MainEventManager::_timercallback>(this);
        _timeout.set(timeout, 0);
        _timeout.start();
    }

 private:
    /* internal event loop variable */
    struct ev_loop *_libev_loop;

    /* Internal data structures
        for managing the triggered asynchronous event */
    std::queue<AsyncEvent> _eventQueue;

    /* Internally handling async events whose value is less than _intervalHanldeEventEnum */
    AsyncEventEnum _internalHandleEventEnum;

    /*********************************************************/
    /* Singleton Pattern */
    /*********************************************************/
 public:
    static MainEventManager *Instance();

    static void InitInstance(AsyncEventEnum internalEventEnum = AsyncEventEnum::Base);

    static void InitInstance(double timeout, AsyncEventEnum internalEventEnum = AsyncEventEnum::Base);

 protected:
    explicit MainEventManager(AsyncEventEnum internalEventEnum = AsyncEventEnum::Base);
    MainEventManager(double timeout, AsyncEventEnum internalEventEnum = AsyncEventEnum::Base);

 private:
    static MainEventManager *_instance;

 public:
    /*********************************************************/
    /* Public API designed for main event loop controls */
    /*********************************************************/
    /* blocking API that awaits for next asynchronous event */
    void Wait();
    void PushAsyncEvent(AsyncEvent event);
    bool ExistAsyncEvent() { return !_eventQueue.empty(); }
    AsyncEvent PopAsyncEvent();
};
}   // namespace libBLEEP_BL

#endif   // BLEEPLIB_SRC_BL_MAINEVENTMANAGER_H_
