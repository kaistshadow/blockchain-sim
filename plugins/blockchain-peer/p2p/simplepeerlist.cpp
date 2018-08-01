#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <time.h>

#include "simplepeerlist.h"
#include "nodeinfos.h"


PeerList inPeerList = std::vector<Peer>();
PeerList outPeerList = std::vector<Peer>();


// TODO : Each node should open outgoing socket and ingoing sockets 

static int initialize_server (int port) {
    // initialize server socket
    // need to be corrected
    int server_sd = socket(AF_INET, (SOCK_STREAM), 0);

    /* setup the socket address info */
    struct sockaddr_in bindAddress;
    memset(&bindAddress, 0, sizeof(bindAddress));
    bindAddress.sin_family      = AF_INET;
    bindAddress.sin_addr.s_addr = INADDR_ANY;
    bindAddress.sin_port        = htons(port); 
    
    /* bind the socket to the server port */
    int res = bind(server_sd, (struct sockaddr *) &bindAddress, sizeof(bindAddress));
    if (res == -1) {
        std::cout << "bind error!\n";
        return -1;
    }

    /* set as server socket that will listen for clients */
    res = listen(server_sd, 100);
    if (res == -1) {
        std::cout <<  "unable to start server: error in listen";
        return -1;
    }
    
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int client_sd = accept(server_sd, (struct sockaddr *) &client, &client_len);

    if (client_sd < 0)  {
        std::cout <<  "unable to start server: error in accept";
        return -1;
    }

    return client_sd;
} 

static int connect_to_node (std::string hostname, int port) {
    int s, sfd;

    struct addrinfo* serverInfo;
    s = getaddrinfo(hostname.c_str(), NULL, NULL, &serverInfo);
    if (s != 0) {
        std::cout << "unable to start client: error in getaddrinfo" << std::endl;
        return -1;
    }

    in_addr_t serverIP = ((struct sockaddr_in*) (serverInfo->ai_addr))->sin_addr.s_addr;
    freeaddrinfo(serverInfo);

    /* create the client socket and get a socket descriptor */
    sfd = socket(AF_INET, (SOCK_STREAM), 0);
    if (sfd == -1) {
        std::cout << "unable to start client: error in socket" << std::endl;;
        return -1;
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = serverIP;
    serverAddress.sin_port = htons(port);

    /* connect to server. since we are non-blocking, we expect this to return EINPROGRESS */
    int res = connect(sfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    if (res == -1 && errno != EINPROGRESS) {
        std::cout << "unable to start client: error in connect" << std::endl;
        return -1;
    }
    return sfd;
}

// TODO : Implement more flexible peerlist setup process
// For example, receiving peer ip list from command line interface
// But, How to dynamically change the peerlist after initialization? 
// Check out how real bitcoin client handles this problem.
void InitializePeerList() {
    int server_port, neighbor_port;
    if (GetLocalNodeId() == 0) {
        // initiate server
        server_port = 19000;
        int incoming_sd = initialize_server(server_port);
        inPeerList.push_back(Peer("bleep2", incoming_sd)); // hardcoded hostname for simple shadow test
        std::cout << "inPeer : " << incoming_sd << std::endl;
    }
    else if (GetLocalNodeId() == 1) {
        // connect to neighbor
        std::string neighbor_hostname("bleep1"); // hardcoded hostname for simple shadow test
        neighbor_port = 19000;
        int out_sd = connect_to_node(neighbor_hostname, neighbor_port);        
        outPeerList.push_back(Peer("bleep1", out_sd));
        std::cout << "outPeer : " << out_sd << std::endl;
    }
    
}
