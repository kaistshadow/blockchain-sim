// "Copyright [2021] <kaistshadow>"

#include <fcntl.h> /* Added for the nonblocking socket */
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <memory>

#include "shadow_interface.h"
#include "Socket.h"
#include "../utility/Assert.h"

using namespace libBLEEP_BL;

ListenSocket::ListenSocket(int port) {
    int listenfd;     /* listen on sock_fd */
    struct sockaddr_in my_addr;    /* my address information */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    my_addr.sin_family = AF_INET;         /* host byte order */
    my_addr.sin_port = htons(port);     /* short, network byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
    bzero(&(my_addr.sin_zero), 8);        /* zero the rest of the struct */

    if (bind(listenfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(1);
    }
    if (listen(listenfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    int flags = fcntl(listenfd, F_GETFL, 0);
    fcntl(listenfd, F_SETFL, flags|O_NONBLOCK);
    _fd = listenfd;

    // create event watcher for the ListenSocket
    _watcher = std::unique_ptr<ListenSocketWatcher>(new ListenSocketWatcher(_fd));
}

ListenSocket::ListenSocket(int port, const char* shadow_addr) {
    int listenfd; /* listen on sock_fd */
    struct sockaddr_in my_addr;    /* my address information */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    my_addr.sin_family = AF_INET;         /* host byte order */
    my_addr.sin_port = htons(port);     /* short, network byte order */
    my_addr.sin_addr.s_addr = inet_addr(shadow_addr);
    bzero(&(my_addr.sin_zero), 8);        /* zero the rest of the struct */

    if (shadow_bind(listenfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(1);
    }
    if (listen(listenfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    int flags = fcntl(listenfd, F_GETFL, 0);
    fcntl(listenfd, F_SETFL, flags | O_NONBLOCK);
    _fd = listenfd;

    // create event watcher for the ListenSocket
    _watcher = std::unique_ptr<ListenSocketWatcher>(new ListenSocketWatcher(_fd));
}


int ListenSocket::DoAccept() {
    struct sockaddr_in their_addr; /* connector's address information */
    int sock_fd;
    socklen_t sin_size;

    sin_size = sizeof(struct sockaddr_in);
    sock_fd = accept(_fd, (struct sockaddr *)&their_addr, &sin_size);
    if ( sock_fd != -1 ) {
        std::cout << "server: got connection from " << inet_ntoa(their_addr.sin_addr) << "\n";

        fcntl(sock_fd, F_SETFL, O_NONBLOCK);

        return sock_fd;
    } else {
        if ((errno != EAGAIN) && (errno != EWOULDBLOCK)) {
            std::cout << "accept() failed errno=" << errno << strerror(errno) << "\n";
            exit(-1);
        }
        return -1;
    }
}

ListenSocket::~ListenSocket() {
    if (close(_fd) == -1) {
        perror("close");
        libBLEEP::M_Assert(0, "error on close listen socket");
    } else {
        std::cout << "ListenSocket(" << _fd << ") closed" << "\n";
    }
}

ConnectSocket::ConnectSocket(std::string domain) : _domain(domain) {
    int remote_fd;
    if ((remote_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        libBLEEP::M_Assert(0, "error on open connect socket");
    }

    int flags = fcntl(remote_fd, F_GETFL, 0);
    fcntl(remote_fd, F_SETFL, flags | O_NONBLOCK);

    struct addrinfo* servinfo;
    int n = getaddrinfo(domain.c_str(), NULL, NULL, &servinfo);
    if (n != 0) {
        std::cout << "Value of errno: " << errno << "\n";
        fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(n));
        perror("getaddrinfo");
        std::cout << "domain" << domain << "\n";
        libBLEEP::M_Assert(0, "error : getaddrinfo ");
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(DEFAULT_SOCKET_PORT);
    servaddr.sin_addr.s_addr = ((struct sockaddr_in*) (servinfo->ai_addr))->sin_addr.s_addr;

    n = connect(remote_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if (n < 0 && errno != EINPROGRESS) {
        perror("connect");
        std::cout << "Unable to connect to " << domain << "\n";
        libBLEEP::M_Assert(0, "connect error returned");
    } else if (n == 0) {
        std::cout << "connection established" << "\n";
        libBLEEP::M_Assert(0, "non-blocking socket is immediately suceeded. is it possible?");
    } else if (n > 0) {
        libBLEEP::M_Assert(0, "unexpected return value of non-blocking connect");
    }

    _fd = remote_fd;

    // create event watcher for the ConnectSocket
    _watcher = std::unique_ptr<ConnectSocketWatcher>(new ConnectSocketWatcher(_fd));
}

DataSocket::DataSocket(int sfd) {
    _fd = sfd;

    // create event watcher for the DataSocket
    _watcher = std::unique_ptr<DataSocketWatcher>(new DataSocketWatcher(_fd));
}

DataSocket::~DataSocket() {
    if (close(_fd) == -1) {
        perror("close");
        libBLEEP::M_Assert(0, "error on close data socket");
    }
    std::cout << "data socket closed" << "\n";
}

void DataSocket::AppendToSendBuff(const char *buf, int size) {
    _sendBuff.push_back(std::make_shared<WriteMsg>(buf, size));
    _watcher->SetWritable();
}

void DataSocket::DoSend() {
    if (_sendBuff.empty()) {
        _watcher->UnsetWritable();
    }

    while (!_sendBuff.empty()) {
        std::shared_ptr<WriteMsg> msg = _sendBuff.front();
        int numbytes = send(_fd, msg->dpos(), msg->nbytes(), 0);
        if (numbytes < 0) {
            perror("write error");
            exit(-1);
        }

        msg->pos += numbytes;
        if (msg->nbytes() == 0) {
            _sendBuff.pop_front();
            if (_sendBuff.empty()) {
                _watcher->UnsetWritable();
                std::cout << "all sent" << "\n";
                return;
            }
        } else {
            return;
        }
    }
}
