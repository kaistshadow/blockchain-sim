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


#include <map>
#include <vector>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>


#include "blockchain/transaction.h"
#include "p2p/p2pmessage.h"
#include "p2p/socketmessage.h"
#include "consensus/simpleconsensusmessage.h"
#include "p2p/socket.h"
#include "p2p/plumtree.h"

#define MYPORT 3456    /* the port users will be connecting to */
#define BACKLOG 10     /* how many pending connections queue will hold */

using namespace std;

enum BLOCKING_CONNECT_STATUS {
    IDLE = 0,
    CONNECTED = 1,
};

int client_sfd;
map< string, int > socket_m;

void NodeInit(int argc, char *argv[]);

void NodeLoop(char *argv[]);

int main(int argc, char *argv[]) {
    // int nodeid = atoi(argv[1]);
    cout << "powconsensus injector started!\n";

    NodeInit(argc, argv);
    NodeLoop(argv);
}

void connect_to_node(char *hostname) {

    // blocking connect
    int 			cli_sockfd;
    if ((cli_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in servaddr;
    struct addrinfo* servinfo;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);

    int n = getaddrinfo(hostname, NULL, NULL, &servinfo);
    if (n != 0) {
        cout << "error in connection : getaddrinfo" << "\n";
        exit(1);
    }
    servaddr.sin_addr.s_addr = ((struct sockaddr_in*) (servinfo->ai_addr))->sin_addr.s_addr;

    n = connect(cli_sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    if (n < 0) {
        perror("connect");
        exit(1);
    } 
    else if (n == 0) {
        cout << "connection established" << "\n";
        socket_m.insert(make_pair(hostname, cli_sockfd));
    }

    return;
}

void send_SocketMessage(int sfd, SocketMessage msg) {
    std::string payload = GetSerializedString(msg.GetP2PMessage());
    int     payload_length = payload.size();

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
            
    n = send(sfd,payload.c_str(),payload_length,0);
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
    return;
}

void InjectTransaction(int sfd, int from, int to, double value) {
    Transaction tx(from, to, value);
    P2PMessage p2pmessage(P2PMessage_TRANSACTION, tx);

    // added
    p2pmessage.g_mid = GossipProtocol::GetInstance()->CreateMsgId(p2pmessage);

    SocketMessage msg;
    msg.SetSocketfd(sfd);
    msg.SetP2PMessage(p2pmessage);
    // std::string payload = GetSerializedString(msg);
    // msg.SetPayload(payload);

    cout << "Following tx will be injected" << "\n";
    cout << tx << "\n";

    send_SocketMessage(sfd, msg);
}

void NodeInit(int argc, char *argv[]) {

    for (int i = 1; i < argc; i++) {
        connect_to_node(argv[i]);
    }
}


void NodeLoop(char *argv[]) {
    int n;
    
    vector<int> client_sfd_list; 

    {
        // 1. create Transaction and inject to first node
        int sfd = socket_m.find(argv[1])->second;
        InjectTransaction(sfd, 0,1,12.34); // send 12.34 to node 1 from node 0
        InjectTransaction(sfd, 0,2,10);
        InjectTransaction(sfd, 1,3,10);
        InjectTransaction(sfd, 2,0,11);
        InjectTransaction(sfd, 1,0,12.34);

        InjectTransaction(sfd, 1,0,22.34); 
        InjectTransaction(sfd, 2,0,20);
        InjectTransaction(sfd, 3,1,20);
        InjectTransaction(sfd, 0,3,22);
        InjectTransaction(sfd, 1,2,22.22);

    }

    return;
}
