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


SimplePeerList* SimplePeerList::instance = 0;

SimplePeerList* SimplePeerList::GetInstance() {
    if (instance == 0) {
        instance = new SimplePeerList();
    }
    return instance;
}


// TODO : Each node should open outgoing socket and ingoing sockets 

// static int initialize_server (int port) {
//     // initialize server socket
//     // need to be corrected
//     int server_sd = socket(AF_INET, (SOCK_STREAM), 0);

//     /* setup the socket address info */
//     struct sockaddr_in bindAddress;
//     memset(&bindAddress, 0, sizeof(bindAddress));
//     bindAddress.sin_family      = AF_INET;
//     bindAddress.sin_addr.s_addr = INADDR_ANY;
//     bindAddress.sin_port        = htons(port); 
    
//     /* bind the socket to the server port */
//     int res = bind(server_sd, (struct sockaddr *) &bindAddress, sizeof(bindAddress));
//     if (res == -1) {
//         std::cout << "bind error!\n";
//         return -1;
//     }

//     /* set as server socket that will listen for clients */
//     res = listen(server_sd, 100);
//     if (res == -1) {
//         std::cout <<  "unable to start server: error in listen";
//         return -1;
//     }
    
//     struct sockaddr_in client;
//     socklen_t client_len = sizeof(client);
//     int client_sd = accept(server_sd, (struct sockaddr *) &client, &client_len);

//     if (client_sd < 0)  {
//         std::cout <<  "unable to start server: error in accept";
//         return -1;
//     }

//     return client_sd;
// } 

// static int connect_to_node (std::string hostname, int port) {
//     int s, sfd;

//     struct addrinfo* serverInfo;
//     s = getaddrinfo(hostname.c_str(), NULL, NULL, &serverInfo);
//     if (s != 0) {
//         std::cout << "unable to start client: error in getaddrinfo" << "\n";
//         return -1;
//     }

//     in_addr_t serverIP = ((struct sockaddr_in*) (serverInfo->ai_addr))->sin_addr.s_addr;
//     freeaddrinfo(serverInfo);

//     /* create the client socket and get a socket descriptor */
//     sfd = socket(AF_INET, (SOCK_STREAM), 0);
//     if (sfd == -1) {
//         std::cout << "unable to start client: error in socket" << "\n";;
//         return -1;
//     }

//     struct sockaddr_in serverAddress;
//     memset(&serverAddress, 0, sizeof(serverAddress));
//     serverAddress.sin_family = AF_INET;
//     serverAddress.sin_addr.s_addr = serverIP;
//     serverAddress.sin_port = htons(port);

//     /* connect to server. since we are non-blocking, we expect this to return EINPROGRESS */
//     int res = connect(sfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
//     if (res == -1 && errno != EINPROGRESS) {
//         std::cout << "unable to start client: error in connect" << "\n";
//         return -1;
//     }
//     return sfd;
// }


// TODO : Implement more flexible peerlist setup process
// For example, receiving peer ip list from command line interface
// But, How to dynamically change the peerlist after initialization? 
// Check out how real bitcoin client handles this problem.

// InitializePeerList : make outPeer for given hostname. 
// (so, it only supports a single outpeer, currently.)
void SimplePeerList::AddPeerList(char *servhostname) {

    if (strcmp(servhostname, "none") == 0)
        return;

    std::cout << "outPeer : " << servhostname << "\n";
    Peer* outPeer = new Peer(servhostname);
    outPeerList.push_back(outPeer);
}
