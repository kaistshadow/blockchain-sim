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
#include "p2p/centralized/broadcastrequestmessage.h"
#include "p2p/centralized/centralizednetworkmessage.h"
#include "p2p/centralized/peerlistmanager.h"
#include "p2p/centralized/simplesocketinterface.h"


#define MYPORT 3456    /* the port users will be connecting to */
#define BACKLOG 10     /* how many pending connections queue will hold */

using namespace std;


void NodeInit(int argc, char *argv[]);

void NodeLoop(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    // int nodeid = atoi(argv[1]);
    // assume that command line arguments are given as follows.
    // ./INJECTOR <centralized node id> <number of tx that will be injected>
    cout << "powconsensus injector started!\n";

    NodeInit(argc, argv);
    NodeLoop(argc, argv);
}

void InjectTransaction(char *node, int from, int to, double value) {
    Transaction tx(from, to, value);

    BroadcastRequestMessage brmsg(BroadcastRequestMessage_TRANSACTION, tx);
    CentralizedNetworkMessage nmsg(CentralizedNetworkMessage_BROADCASTREQMSG, brmsg);
    SimpleSocketInterface::GetInstance()->SendNetworkMsg(nmsg, std::string(node)); // center is hardcoded to bleep1 for test
    // CentralizedGossipProtocol::GetInstance()->Broadcast(p2pmessage);

    cout << "Following tx is injected" << "\n";
    cout << tx << "\n";
}

void NodeInit(int argc, char *argv[]) {
    // 1. Initialize the list of out peers 
    SimplePeerListManager::GetInstance()->InitializeOutPeerList(1, &argv[1]);

    // 2. Initialize network socket
    PeerList& outPeerList = SimplePeerListManager::GetInstance()->GetOutPeerList();
    for (Peer* p : outPeerList) {
        int 			cli_sockfd;
        if ((cli_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            exit(1);
        }
        p->sfd = cli_sockfd;

        struct sockaddr_in servaddr;
        struct addrinfo* servinfo;
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(MYPORT);

        int n = getaddrinfo(p->hostname.c_str(), NULL, NULL, &servinfo);
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
            p->conn_status = CONNECTED;
        }
    }
}


void NodeLoop(int argc, char *argv[]) {
    int n;
    
    int inject_num = atoi(argv[argc-1]);
    char *center_node = argv[1];

    {
        // 1. create Transaction and broadcast
        for (int i = 1; i <= inject_num; i++) {
            float amount = 0.1 * i;
            InjectTransaction(center_node, 0,1,amount); // send 12.34 to node 1 from node 0
            InjectTransaction(center_node, 0,2,amount);
            InjectTransaction(center_node, 1,3,amount);
            InjectTransaction(center_node, 2,0,amount);
            InjectTransaction(center_node, 1,0,amount);
        }
    }
    return;
}
