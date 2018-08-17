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

#define MYPORT 3456    /* the port users will be connecting to */
#define BACKLOG 10     /* how many pending connections queue will hold */

using namespace std;

enum BLOCKING_CONNECT_STATUS {
    IDLE = 0,
    CONNECTED = 1,
};

int client_sfd;

void NodeInit();

void NodeLoop(char *servhostname);

int main(int argc, char *argv[]) {
    // int nodeid = atoi(argv[1]);
    cout << "Nonblock-io injector started!\n";

    NodeInit();
    NodeLoop(argv[1]);
}

void NodeInit() {

    /*******************************  init for client socket start *******************************/

    int 			cli_sockfd;  /* listen on sock_fd, new connection on new_fd */

    if ((cli_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    client_sfd = cli_sockfd;
    
    /*******************************  init for client socket end  *******************************/

    // initialize the list of neighbor nodes
    // InitializePeerList();
}

void NodeLoop(char *servhostname) {
    socklen_t 			sin_size;
    int				last_fd;	/* Thelast sockfd that is connected	*/
    char			string_write[255];
    int n;
    BLOCKING_CONNECT_STATUS conn_status = IDLE;
    
    vector<int> client_sfd_list; // currently, i assumed that this injector only connects to a single peer.
    sprintf(string_write, "This_is_transaction_generated_by_injector");

    while (true) {
        usleep(100000);
        
        // blocking send
        for (vector<int>::iterator it = client_sfd_list.begin(); it != client_sfd_list.end(); it++) {
            int sfd = *it;
            n = send(sfd,string_write,sizeof(string_write),0);
            if (n < 0){ 
                cout << "send errno=" << errno << "\n";
                exit(1);
            }
            else if (n < sizeof(string_write)) {
                cout << "Warning : sented string is less than requested" << "\n";
                cout << "sented string length: " << n << "\n";
            }
            else {
                cout << "sented string length: " << n << "\n";
            }
        }

        // blocking connect
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

            n = connect(client_sfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

            if (n < 0) {
                perror("connect");
                exit(1);
            } 
            else if (n == 0) {
                cout << "connection established" << "\n";
                client_sfd_list.push_back(client_sfd);
                conn_status = CONNECTED;
            }
            break;
        case CONNECTED:
            break;
        }
    }
    return;
}
