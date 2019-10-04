#include <iostream>
#include <unistd.h>
#include <fcntl.h> /* Added for the nonblocking socket */
#include <arpa/inet.h>

#include "SocketLayer.h"

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
        // TODO : create new data socket
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
        _socketManager.RemoveConnectSocket(fd);
        close(fd);

        // TODO create event for connectSocket error?

        return;
    }
            
    std::cout << "Socket is successfully connected" << "\n";

    // TODO create new data socket


    // remove ConnectSocket
    _socketManager.RemoveConnectSocket(fd);
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
    }
}

void BL_SocketLayer::ConnectSocket(std::string dest) {
    _socketManager.CreateNonblockConnectSocket(dest);
}

