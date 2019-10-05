#include <iostream>
#include <unistd.h>
#include <fcntl.h> /* Added for the nonblocking socket */
#include <arpa/inet.h>

#include "SocketLayer.h"
#include "../utility/Assert.h"

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

    // TODO : push event 'DataSocketInitialized'
}

void BL_SocketLayer::RecvHandler(int fd) {
    std::cout << "DoRecv!" << "\n";
    char string_read[2001];
    int n;
    memset(string_read, 0, 2000);
    while (1) {
        n = recv(fd, string_read, 2000, 0);
        if (n == 2000) {
            std::cout << "received data:[" << string_read << "]" << "\n";
            memset(string_read, 0, 2000);
            continue;
        }
        else if (0 < n) {
            std::cout << "received data:[" << string_read << "]" << "\n";
            break;
        }
        else if (n == 0) {
            std::cout << "connection closed while recv" << "\n";
            // TODO : notify close event?
            _socketManager.RemoveDataSocket(fd);
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

void BL_SocketLayer::ConnectSocket(std::string dest) {
    _socketManager.CreateNonblockConnectSocket(dest);
}

void BL_SocketLayer::SendToSocket(int fd, char* buf, int size) {
    auto socket = _socketManager.GetDataSocket(fd);
    if (socket == nullptr) 
        libBLEEP::M_Assert(0, "No valid dataSocket exists for sending");

    socket->AppendToSendBuff(buf, size);
}
