#include <iostream>

#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/wait.h> 
#include <fcntl.h> /* Added for the nonblocking socket */
#include <arpa/inet.h>
#include <netdb.h>

#include <vector>


#include "socket.h"
#include "simplepeerlist.h"

SocketInterface* SocketInterface::instance = 0;

SocketInterface* SocketInterface::GetInstance() {
    if (instance == 0) {
        instance = new SocketInterface();
    }
    return instance;
}


void SocketInterface::InitServerSocket() {
    int 			sockfd;     /* listen on sock_fd */
    struct 	sockaddr_in 	my_addr;    /* my address information */
    int 			sin_size;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    my_addr.sin_family = AF_INET;         /* host byte order */
    my_addr.sin_port = htons(MYPORT);     /* short, network byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
    bzero(&(my_addr.sin_zero), 8);        /* zero the rest of the struct */

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    fcntl(sockfd, F_SETFL, O_NONBLOCK); /* Change the socket into non-blocking state	*/

    SetListenSocket(sockfd);
}

void SocketInterface::InitClientSocket(PeerList outPeerList) {

    // initialize socket for client connection
    for (PeerList::iterator it = outPeerList.begin(); it != outPeerList.end(); it++) {    
        Peer* p = *it;
        int cli_sockfd;  

        if ((cli_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            exit(1);
        }
    
        int flags = fcntl(cli_sockfd, F_GETFL, 0);
        fcntl(cli_sockfd, F_SETFL, flags | O_NONBLOCK); /* Change the socket into non-blocking state	*/

        p->sfd = cli_sockfd;
    }
}

/**
 * Initialize the non-blocking socket for listening socket and client socket.
 */
void SocketInterface::InitializeSocket(PeerList outPeerList) {
    InitServerSocket();

    InitClientSocket(outPeerList);
}

/**
 * Process socket events
 * 1. process non-blocking accept (and make recv_sockets)
 * 2. process non-blocking connect (and make send_sockets)
 * 3. process non-blocking recv
 */
void SocketInterface::ProcessNonblockSocket(PeerList outPeerList) {
    struct 	sockaddr_in 	their_addr; /* connector's address information */
    int                         new_fd; /* new connection on new_fd */
    socklen_t 			sin_size;
    char			string_read[255];
    int n;
    
    // 1. Process non-blocking accept
    sin_size = sizeof(struct sockaddr_in);
    new_fd = accept(GetListenSocket(), (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1 && errno != EAGAIN) {
        perror("accept");
        cout << "errno=" << errno << "\n";
    }
    else if (new_fd != -1) {
        cout << "server: got connection from " << inet_ntoa(their_addr.sin_addr) << "\n"; 
        fcntl(new_fd, F_SETFL, O_NONBLOCK);
        recv_sfd_list.push_back(new_fd); 
    }
        
    // 2. Process non-blocking connect
    for (PeerList::iterator it = outPeerList.begin(); it != outPeerList.end(); it++) {    
        Peer* p = *it;

        switch (p->conn_status) {
        case IDLE:
            struct sockaddr_in servaddr;
            struct addrinfo* servinfo;
            bzero(&servaddr, sizeof(servaddr));
            servaddr.sin_family = AF_INET;
            servaddr.sin_port = htons(MYPORT);

            n = getaddrinfo(p->hostname.c_str(), NULL, NULL, &servinfo);
            if (n != 0) {
                cout << "error in connection : getaddrinfo" << "\n";
                exit(1);
            }
            servaddr.sin_addr.s_addr = ((struct sockaddr_in*) (servinfo->ai_addr))->sin_addr.s_addr;

            // if (inet_pton(AF_INET, servip, &servaddr.sin_addr) <= 0) {
            //     perror("inet_pton");
            //     exit(1);
            // }

            n = connect(p->sfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

            if (n < 0 && errno != EINPROGRESS) {
                perror("connect");
                exit(1);
            } 
            else if (n == 0) {
                cout << "connection established" << "\n";
                p->conn_status = CONNECTED;
                send_sfd_list.push_back(p->sfd);
            }
            else {
                p->conn_status = CONNECTING;
            }
            break;
        case CONNECTING:
            fd_set rset, wset;
            struct timeval tval;

            FD_ZERO(&rset);
            FD_SET(p->sfd, &rset);
            wset = rset;
            tval.tv_sec = 0;
            tval.tv_usec = 0;
            
            n = select(p->sfd+1, &rset, &wset, NULL, &tval);
            if (n == 1) {
                if (FD_ISSET(p->sfd, &rset) || FD_ISSET(p->sfd, &wset)) {
                    int error = 0;
                    socklen_t len = sizeof(error);
                    if (getsockopt(p->sfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
                        perror("getsockopt");
                        exit(1);
                    }

                    if (error) {
                        cout << "connection failed" << "\n";
                        exit(1);
                    }
                    else {
                        cout << "connection established" << "\n";
                        p->conn_status = CONNECTED;
                        send_sfd_list.push_back(p->sfd);
                    }
                }
            }

            break;
        case CONNECTED:
            break;
        }
        
    }

    // 3. Process non-blocking recv
    for (vector<int>::iterator it = recv_sfd_list.begin(); it != recv_sfd_list.end(); it++) {
        int sfd = *it;
        n = recv(sfd,string_read,sizeof(string_read),0);
        if (n == -1 && errno != EAGAIN){ 
            perror("recv - non blocking \n");
            cout << "errno=" << errno << "\n";
            exit(1);
        }
        else if (n == 0) {
            cout << "socket disconnected" << "\n";
        }
        else if (n > 0) {
            string_read[n] = '\0';
            cout << "The string is: " << string_read << "\n";
        }
    }
}

void SocketInterface::SetListenSocket(int sfd) {
    server_sfd = sfd;
}

int SocketInterface::GetListenSocket() {
    return server_sfd;
}
