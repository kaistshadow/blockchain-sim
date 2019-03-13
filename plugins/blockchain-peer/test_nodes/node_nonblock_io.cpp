#include <iostream>
#include <stdlib.h>
#include <unistd.h>

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

// #include "p2p/simplepeerlist.h"
// #include "p2p/nodeinfos.h"

#define MYPORT 3456    /* the port users will be connecting to */
#define BACKLOG 10     /* how many pending connections queue will hold */

using namespace std;

enum CONNECT_STATUS {
    IDLE = 0,
    CONNECTING = 1,
    CONNECTED = 2,
};

int server_sfd;
int client_sfd;

void NodeInit();

void NodeLoop(char *servhostname);

int main(int argc, char *argv[]) {
    // int nodeid = atoi(argv[1]);
    cout << "Nonblock-io peer started!\n";

    NodeInit();
    NodeLoop(argv[1]);
}

void NodeInit() {

    /*******************************  init for server socket start *******************************/

    int 			sockfd;     /* listen on sock_fd */
    struct 	sockaddr_in 	my_addr;    /* my address information */
    int 			sin_size;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    server_sfd = sockfd;

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

    fcntl(server_sfd, F_SETFL, O_NONBLOCK); /* Change the socket into non-blocking state	*/

    /*******************************  init for server socket end  ********************************/

    /*******************************  init for client socket start *******************************/

    int 			cli_sockfd;  /* listen on sock_fd, new connection on new_fd */

    if ((cli_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    client_sfd = cli_sockfd;
    
    int flags = fcntl(client_sfd, F_GETFL, 0);
    fcntl(client_sfd, F_SETFL, flags | O_NONBLOCK); /* Change the socket into non-blocking state	*/

    /*******************************  init for client socket end  *******************************/

    // initialize the list of neighbor nodes
    // InitializePeerList();
}

void NodeLoop(char *servhostname) {
    struct 	sockaddr_in 	their_addr; /* connector's address information */
    int                         new_fd; /* new connection on new_fd */
    socklen_t 			sin_size;
    int				last_fd;	/* Thelast sockfd that is connected	*/
    char			string_read[255];
    int n;
    CONNECT_STATUS conn_status = IDLE;
    
    vector<int> sfd_list;

    while (true) {
        usleep(100000);
        
        // non-blocking accept
        sin_size = sizeof(struct sockaddr_in);
        new_fd = accept(server_sfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1 && errno != EAGAIN) {
            perror("accept");
            cout << "errno=" << errno << "\n";
        }
        else if (new_fd != -1) {
            cout << "server: got connection from " << inet_ntoa(their_addr.sin_addr) << "\n"; 
            fcntl(new_fd, F_SETFL, O_NONBLOCK);
            sfd_list.push_back(new_fd);                
        }
        
        // non-blocking recv
        for (vector<int>::iterator it = sfd_list.begin(); it != sfd_list.end(); it++) {
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

        // non-blocking connect
        switch (conn_status) {
        case IDLE:
            struct sockaddr_in servaddr;
            struct addrinfo* servinfo;
            bzero(&servaddr, sizeof(servaddr));
            servaddr.sin_family = AF_INET;
            servaddr.sin_port = htons(MYPORT);

            n = getaddrinfo(servhostname, NULL, NULL, &servinfo);
            if (n != 0) {
                cout << "error in connection : getaddrinfo" << "\n";
                exit(1);
            }
            servaddr.sin_addr.s_addr = ((struct sockaddr_in*) (servinfo->ai_addr))->sin_addr.s_addr;

            // if (inet_pton(AF_INET, servip, &servaddr.sin_addr) <= 0) {
            //     perror("inet_pton");
            //     exit(1);
            // }

            n = connect(client_sfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

            if (n < 0 && errno != EINPROGRESS) {
                perror("connect");
                exit(1);
            } 
            else if (n == 0) {
                cout << "connection established" << "\n";
                conn_status = CONNECTED;
            }
            else {
                conn_status = CONNECTING;
            }
            break;
        case CONNECTING:
            fd_set rset, wset;
            struct timeval tval;

            FD_ZERO(&rset);
            FD_SET(client_sfd, &rset);
            wset = rset;
            tval.tv_sec = 0;
            tval.tv_usec = 0;
            
            n = select(client_sfd+1, &rset, &wset, NULL, &tval);
            if (n == 1) {
                if (FD_ISSET(client_sfd, &rset) || FD_ISSET(client_sfd, &wset)) {
                    int error = 0;
                    socklen_t len = sizeof(error);
                    if (getsockopt(client_sfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
                        perror("getsockopt");
                        exit(1);
                    }

                    if (error) {
                        cout << "connection failed" << "\n";
                        exit(1);
                    }
                    else {
                        cout << "connection established" << "\n";
                        conn_status = CONNECTED;
                    }
                }
            }

            break;
        case CONNECTED:
            break;
        }
    }
    return;
}
