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
#include "consensus/stellarquorum.h"
#include "consensus/stellarconsensusmessage.h"

#define MYPORT 3456    /* the port users will be connecting to */
#define BACKLOG 10     /* how many pending connections queue will hold */

using namespace std;

int client_sfd;
map< string, int > socket_m;

void NodeInit(int argc, char *argv[]);

void NodeLoop(char *argv[]);

int main(int argc, char *argv[]) {
    // int nodeid = atoi(argv[1]);
    cout << "stellarconsensus injector started!\n";

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
    return;
}

// assume that command arguments are given as follows
// ./command <node_id_of_neighbor> <node_id_of_neighbor> ...
void NodeInit(int argc, char *argv[]) {

    cout << "argc:" << argc << "\n";
    for (int i = 1; i < argc; i++) {
        cout << "connect to " << argv[i] << "\n";
        connect_to_node(argv[i]);
    }
}

void NodeLoop(char *argv[]) {

    {
        // 1. Quorums & Quorum Slices initialization (Stellar paper Figure.7)
        StellarQuorumSlices slices;
        set<string> s1 = {"bleep1", "bleep2", "bleep3", "bleep7"};  // C++11 feature
        set<string> s2 = {"bleep4", "bleep5", "bleep6", "bleep7"};
        set<string> s3 = {"bleep7"};
        slices.AddQuorumSlice("bleep1", s1);
        slices.AddQuorumSlice("bleep2", s1);
        slices.AddQuorumSlice("bleep3", s1);
        slices.AddQuorumSlice("bleep4", s2);
        slices.AddQuorumSlice("bleep5", s2);
        slices.AddQuorumSlice("bleep6", s2);
        slices.AddQuorumSlice("bleep7", s3);

        StellarQuorums quorums;
        set<string> q1 = {"bleep1", "bleep2", "bleep3", "bleep7"};  // C++11 feature
        set<string> q2 = {"bleep4", "bleep5", "bleep6", "bleep7"};
        set<string> q3 = {"bleep7"};
        quorums.AddQuorum(q1);
        quorums.AddQuorum(q2);
        quorums.AddQuorum(q3);

        for (auto it = socket_m.begin(); it != socket_m.end(); it++) {
            std::string node_id = it->first;
            int sfd = it->second;
            StellarConsensusMessage consensusMsg;
            consensusMsg.type = StellarConsensusMessage_INIT_QUORUM;
            SCPInit scpInitMsg;
            scpInitMsg.quorums = quorums;
            scpInitMsg.slices = slices;
            consensusMsg.msg = scpInitMsg;
            
            P2PMessage p2pmessage(P2PMessage_STELLARCONSENSUSMESSAGE, consensusMsg);
            SocketMessage msg;
            msg.SetSocketfd(sfd);
            msg.SetP2PMessage(p2pmessage);
            std::string payload = GetSerializedString(msg);
            msg.SetPayload(payload);

            SocketMessage msg2 = GetDeserializedMsg(payload);
            // cout << "Following StellarConsensusMessage is deserialized (with boost variant)" << "\n";
            StellarConsensusMessage deserialized_msg = boost::get<StellarConsensusMessage>(msg2.GetP2PMessage().data);
            SCPInit initMsg = boost::get<SCPInit>(deserialized_msg.msg);
            cout << initMsg.quorums;
            cout << initMsg.slices;
         
            send_SocketMessage(sfd, msg);
        }
    }    

}
