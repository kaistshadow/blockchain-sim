#include "Socket.h"
#include "../utility/Assert.h"

#include <fcntl.h> /* Added for the nonblocking socket */
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>

#include <iostream>


using namespace libBLEEP;

libBLEEP::ListenSocket::ListenSocket(int port) {
    int 			listenfd;     /* listen on sock_fd */
    struct 	sockaddr_in 	my_addr;    /* my address information */
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
    fcntl(listenfd, F_SETFL, flags | O_NONBLOCK); /* Change the socket into non-blocking state	*/
    
    _fd = listenfd;
}

int libBLEEP::ListenSocket::DoAccept() {
    struct 	sockaddr_in 	their_addr; /* connector's address information */
    int sock_fd;
    socklen_t 			sin_size;

    sin_size = sizeof(struct sockaddr_in);
    sock_fd = accept(_fd, (struct sockaddr *)&their_addr, &sin_size);
    if ( sock_fd != -1 ) {
        std::cout << "server: got connection from " << inet_ntoa(their_addr.sin_addr) << "\n";

        fcntl(sock_fd, F_SETFL, O_NONBLOCK);

        return sock_fd;
    }
    else {
        if( errno != EAGAIN && errno != EWOULDBLOCK ) {
            std::cout << "accept() failed errno=" << errno << strerror(errno) << "\n";
            exit(-1);
        }
        return -1;
    }
}

libBLEEP::ListenSocket::~ListenSocket() {
    if ( close(_fd) == -1) {
        perror("close");
        M_Assert(0, "error on close listen socket");
    }
}


libBLEEP::ConnectSocket::ConnectSocket(std::string domain) {
    int remote_fd;
    if ( (remote_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        M_Assert(0, "error on open connect socket");
    }

    int flags = fcntl(remote_fd, F_GETFL, 0);
    fcntl(remote_fd, F_SETFL, flags | O_NONBLOCK); /* Change the socket into non-blocking state	*/

    struct addrinfo* servinfo;
    int n = getaddrinfo(domain.c_str(), NULL, NULL, &servinfo);
    if (n != 0) {
        std::cout << "Value of errno: " << errno << "\n";
        fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(n));
        perror("getaddrinfo");
        std::cout << "domain" << domain << "\n";
        M_Assert(0, "error : getaddrinfo ");
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
        M_Assert(0, "connect error returned");
    }
    else if (n == 0) {
        std::cout << "connection established" << "\n";
        M_Assert(0, "non-blocking socket is immediately suceeded. is it possible?");
    }
    else if (n > 0) {
        M_Assert(0, "unexpected return value of non-blocking connect");
    }

    _fd = remote_fd;
}

libBLEEP::DataSocket::~DataSocket() {
    if ( close(_fd) == -1) {
        perror("close");
        M_Assert(0, "error on close data socket");
    }
}

