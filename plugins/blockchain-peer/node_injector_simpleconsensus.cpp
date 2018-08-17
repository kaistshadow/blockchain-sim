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
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>


#include "blockchain/transaction.h"
#include "p2p/p2pmessage.h"
#include "p2p/socketmessage.h"


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
    cout << "simpleconsensus injector started!\n";

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
    int n;
    BLOCKING_CONNECT_STATUS conn_status = IDLE;
    
    vector<int> client_sfd_list; // currently, i assumed that this injector only connects to a single peer.

    // blocking connect
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

    // blocking send
    for (vector<int>::iterator it = client_sfd_list.begin(); it != client_sfd_list.end(); it++) {
        int sfd = *it;

        // create SocketMessage and insert into the queue
        Transaction tx(0,1,12.34); // send 100.0 to node 1 from node 0
        P2PMessage p2pmessage(P2PMessage_TRANSACTION, tx);
        // SocketMessage msg(sfd, p2pmessage);
        SocketMessage msg;
        msg.SetSocketfd(sfd);
        msg.SetP2PMessage(p2pmessage);
        std::string payload = GetSerializedString(msg);
        msg.SetPayload(payload);

        cout << "Following tx will be injected" << "\n";
        cout << tx << "\n";

        SocketMessage msg2 = GetDeserializedMsg(payload);

        cout << "Following tx is deserialized" << "\n";
        cout << msg2.GetP2PMessage().tx << "\n";

            
        int payload_length = msg.GetPayloadLength();

        int n = send(sfd, (char*)&payload_length, sizeof(int), 0);
        if (n < 0){ 
            cout << "send errno=" << errno << "\n";
            exit(1);
        }
        else if (n < sizeof(int)) {
            cout << "Warning : sented string is less than requested" << "\n";
            cout << "sented string length: " << n << "\n";
            exit(1);
        }
        else {
            cout << "sented string length: " << n << "\n";
        }            
            
        n = send(sfd,msg.GetPayload().c_str(),payload_length,0);
        if (n < 0){ 
            cout << "send errno=" << errno << "\n";
            exit(1);
        }
        else if (n < payload_length) {
            cout << "Warning : sented string is less than requested" << "\n";
            cout << "sented string length: " << n << "\n";
            exit(1);
        }
        else {
            cout << "sented string length: " << n << "\n";
        }            
    }

    return;
}
