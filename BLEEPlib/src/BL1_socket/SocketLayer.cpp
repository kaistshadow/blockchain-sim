#include <iostream>
#include <unistd.h>
#include <fcntl.h> /* Added for the nonblocking socket */
#include <arpa/inet.h>

#include "SocketLayer.h"
#include "../utility/Assert.h"


#include "../BL2_peer_connectivity/Message.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/serialization/export.hpp>

using namespace libBLEEP_BL;


BL_SocketLayer::BL_SocketLayer() {
    _socketManager.CreateListenSocket();

}

void BL_SocketLayer::AcceptHandler(int fd) {
    std::shared_ptr<ListenSocket> listenSocket = _socketManager.GetListenSocket(fd);

    while (1) {
        int data_sfd = listenSocket->DoAccept();
        if (data_sfd == -1)
            break;

        std::cout << "Socket is successfully accepted" << "\n";

        // create new data socket
        _socketManager.CreateDataSocket(data_sfd);
    }
}

void BL_SocketLayer::ConnectHandler(int fd) {
    // check status of socket
    int err = 0;
    socklen_t len = sizeof(err);
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0 ) {
        perror("getsockopt"); // Solaris pending error?
        exit(-1);
    }

    if (err) {
        std::cout << "error for SocketConnect" << "\n";

        // create event for connectSocket error
        auto sock = _socketManager.GetConnectSocket(fd);
        if (sock) {
            AsyncEvent event(AsyncEventEnum::SocketConnectFailed);
            event.GetData().SetFailedSocket(fd);
            event.GetData().SetFailedDomain(sock->GetDomain());
            g_mainEventManager->PushAsyncEvent(event);
        }

        _socketManager.RemoveConnectSocket(fd);
        close(fd);

        return;
    }
            
    std::cout << "Socket is successfully connected" << "\n";

    // successfully connected, so create new data socket
    _socketManager.CreateDataSocket(fd);

    // remove ConnectSocket
    _socketManager.RemoveConnectSocket(fd);

    // TODO : push event 'PeerSocketConnect'
    AsyncEvent event(AsyncEventEnum::PeerSocketConnect);
    event.GetData().SetDataSocket(_socketManager.GetDataSocket(fd));
    g_mainEventManager->PushAsyncEvent(event);
    
}

void BL_SocketLayer::RecvHandler(int fd) {
    std::cout << "DoRecv!" << "\n";

    // recv from socket, and append received data into the buffer.
    char string_read[2001];
    int n;
    memset(string_read, 0, 2000);
    while (1) {
        n = recv(fd, string_read, 2000, 0);
        if (n == 2000) {
            _recvBuffManager.AppendToBuffer(fd, string_read, n);
            std::cout << "received data:[" << string_read << "]" << "\n";
            memset(string_read, 0, 2000);
            continue;
        }
        else if (0 < n) {
            _recvBuffManager.AppendToBuffer(fd, string_read, n);
            std::cout << "received data:[" << string_read << "]" << "\n";
            break;
        }
        else if (n == 0) {
            std::cout << "connection closed while recv" << "\n";
            // TODO : notify socketClose event?
            AsyncEvent event(AsyncEventEnum::PeerSocketClose);
            event.GetData().SetClosedSocket(_socketManager.GetDataSocket(fd));
            g_mainEventManager->PushAsyncEvent(event);

            _socketManager.RemoveDataSocket(fd);
            _recvBuffManager.RemoveRecvBuffer(fd);
            break;
        }
        else if (n < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                std::cout << "recv failed errno=" << errno << strerror(errno) << "\n";
                exit(-1);
            }
            break;
        }
    }

    // TODO : parsing a msg
    std::shared_ptr<SocketRecvBuffer> recvBuffer =  _recvBuffManager.GetRecvBuffer(fd);
    if (recvBuffer) {
        // strncmp(str, substr, strlen(substr)) will return 0 if str starts with substr.
        // Check if the received string starts with the bleep magic value.

        while (true) {
            const char *recvBuf = recvBuffer->recv_str.c_str();
            if (!strncmp(recvBuf, BLEEP_MAGIC, BLEEP_MAGIC_SIZE)) {
                std::cout << "bleep magic received" << "\n";

                // retrieve the size of the msg if possible
                int msg_size = 0;
                if (recvBuffer->recv_str.size() >= BLEEP_MAGIC_SIZE + sizeof(int)) {
                    memcpy(&msg_size, recvBuf+BLEEP_MAGIC_SIZE, sizeof(int));
                    std::cout << "msg length received : " << msg_size <<"\n";
                }
                else
                    break;

                // recv entire msg if possible
                if (msg_size && recvBuffer->recv_str.size() >= BLEEP_MAGIC_SIZE + sizeof(int) + msg_size) {

                    std::cout << "start deserializing MSG" << "\n";
                    recvBuf = recvBuffer->recv_str.c_str();
                    recvBuf += BLEEP_MAGIC_SIZE + sizeof(int);
                    std::shared_ptr<Message> msg;
                    boost::iostreams::basic_array_source<char> device(recvBuf, msg_size);
                    boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
                    boost::archive::binary_iarchive ia(s);
                    ia >> msg;

                    std::cout << "deserializing MSG complete" << "\n";
                    std::cout << msg->GetType() << "\n";

                    // TODO : Each msg should be handled as separate event, and switched to proper layers
                    if (msg->GetType() == "notifyPeerId") {
                        AsyncEvent event(AsyncEventEnum::PeerRecvNotifyPeerId);
                        event.GetData().SetNeighborId(msg->GetSource());
                        event.GetData().SetIncomingSocket(_socketManager.GetDataSocket(fd));
                        g_mainEventManager->PushAsyncEvent(event);
                    }
                    else if (msg->GetType() == "GETADDR") {
                        // GETADDR message is handled by generic (Layer2) PeerRecvMsg event
                    }
                    else if (msg->GetType() == "ADDR") {
                        // ADDR message is handled by generic (Layer2) PeerRecvMsg event
                    }
                    else if (msg->GetType().find("TXGOSSIP", 0) == 0) {
                        // TXGOSSIP protocol message are handled by generic (Layer2) PeerRecvMsg event
                    }
                    // If any new message is added, new statement should be added here. 
                    // (This is for integrity check)
                    else 
                        libBLEEP::M_Assert(0, "Unexpected message");
                    

                    AsyncEvent event(AsyncEventEnum::PeerRecvMsg);
                    event.GetData().SetMsgSourceId(msg->GetSource());
                    event.GetData().SetMsg(msg);
                    g_mainEventManager->PushAsyncEvent(event);

                    // TODO : recvBuffer should be updated efficiently. (minimizing a duplication)
                    std::string remain = recvBuffer->recv_str.substr(BLEEP_MAGIC_SIZE + sizeof(int) + msg_size);
                    recvBuffer->recv_str = remain;
                }
                else
                    break;
            }
            else 
                break;
        }
    }
}

void BL_SocketLayer::WriteHandler(int fd) {
    auto dataSocket = _socketManager.GetDataSocket(fd);
    libBLEEP::M_Assert( dataSocket != nullptr, "dataSocket not exist for given (write) event");
    
    std::cout << "DoSend!" << "\n";
    dataSocket->DoSend();
}


void BL_SocketLayer::SwitchAsyncEventHandler(AsyncEvent& event) {
    switch (event.GetType()) {
    case AsyncEventEnum::SocketAccept:
        {
            int listenfd = event.GetData().GetNewlyAcceptedSocket();
            AcceptHandler(listenfd);
            break;
        }
    case AsyncEventEnum::SocketConnect:
        {
            int fd = event.GetData().GetNewlyConnectedSocket();
            ConnectHandler(fd);
            break;
        }
    case AsyncEventEnum::SocketRecv:
        {
            int fd = event.GetData().GetRecvSocket();
            RecvHandler(fd);
            break;
        }
    case AsyncEventEnum::SocketWrite:
        {
            int fd = event.GetData().GetWriteSocket();
            WriteHandler(fd);
            break;
        }
    }
}

int BL_SocketLayer::ConnectSocket(std::string dest) {
    int conn_socket = _socketManager.CreateNonblockConnectSocket(dest);
    return conn_socket;
}

void BL_SocketLayer::SendToSocket(int fd, const char* buf, int size) {
    auto socket = _socketManager.GetDataSocket(fd);
    if (socket == nullptr) 
        libBLEEP::M_Assert(0, "No valid dataSocket exists for sending");

    socket->AppendToSendBuff((const char*)buf, size);
}

void BL_SocketLayer::DisconnectSocket(int fd) {
    _socketManager.RemoveDataSocket(fd);
    return;
}
