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
#include "p2p/networkmessage.h"
#include "p2p/p2pmessage.h"
#include "p2p/socketmessage.h"
#include "p2p/socket.h"
#include "p2p/plumtree.h"
#include "consensus/simpleconsensusmessage.h"

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
    cout << "simpleconsensus injector started!\n";

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
    NetworkMessage nmsg(NetworkMessage_P2PMSG, msg.GetP2PMessage());
    std::string payload = GetSerializedString(nmsg);
    int     payload_len = payload.size();
    if (payload_len <= 0) {
        std::cerr << "send event: Serialization fault\n";
        return;
    }

    int numbytes = send(sfd, (char*)&payload_len, sizeof(int), 0);
    if (numbytes < sizeof(int)) {
        std::cerr << "send event: network fail\n";
        return;
    }
    numbytes = send(sfd, payload.c_str(), payload_len, 0);
    if (numbytes < payload_len) {
        std::cerr << "send event: network fail\n";
        return;
    }
    std::cout << "send done\n";
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
    }

    {
        // 2. Create Block and inject to first node
        int sfd = socket_m.find(argv[1])->second;
        std::list<Transaction> tx_list;
        Transaction tx(0,1,123.45); // send 123.45 to node 1 from node 0
        tx_list.push_back(tx);
        Transaction tx2(3,2,43.21); // send 43.21 to node 2 from node 3
        tx_list.push_back(tx2);
        Block block("injected block 0", tx_list);
        
        P2PMessage p2pmessage(P2PMessage_BLOCK, block);

        // added
        p2pmessage.g_mid = GossipProtocol::GetInstance()->CreateMsgId(p2pmessage);

        SocketMessage msg;
        msg.SetSocketfd(sfd);
        msg.SetP2PMessage(p2pmessage);
        // std::string payload = GetSerializedString(msg);
        // msg.SetPayload(payload);

        cout << "Following block will be injected" << "\n";
        cout << block << "\n";

        // SocketMessage msg2 = GetDeserializedMsg(payload);
        // cout << "Following block is deserialized (with boost variant)" << "\n";
        // Block deserialized_block = boost::get<Block>(msg2.GetP2PMessage().data);
        // cout << deserialized_block << "\n";

        send_SocketMessage(sfd, msg);
    }

    {
        // 3. Quorum initialization and Leader Election.
        // Remove quorum initialization (node id assigning) process. Since we now have GetHostId function. 

        // for (auto it = socket_m.begin(); it != socket_m.end(); it++) {
        //     std::string node_id = it->first;
        //     int sfd = it->second;
        //     SimpleConsensusMessage consensusMsg(SimpleConsensusMessage_INIT_QUORUM, node_id); 
        //     P2PMessage p2pmessage(P2PMessage_SIMPLECONSENSUSMESSAGE, consensusMsg);

        //     //added
        //     p2pmessage.g_mid = GossipProtocol::GetInstance()->CreateMsgId(p2pmessage);

        //     SocketMessage msg;
        //     msg.SetSocketfd(sfd);
        //     msg.SetP2PMessage(p2pmessage);
        //     // std::string payload = GetSerializedString(msg);
        //     // msg.SetPayload(payload);

        //     send_SocketMessage(sfd, msg);
        // }


        // Broadcast leader election message. 
        std::string node_id(argv[1]); // node0 is elected as a leader
        int sfd = socket_m.find(argv[1])->second;
        SimpleConsensusMessage consensusMsg(SimpleConsensusMessage_LEADER_ELECTION, node_id); 
        P2PMessage p2pmessage(P2PMessage_SIMPLECONSENSUSMESSAGE, consensusMsg);

        //added
        p2pmessage.g_mid = GossipProtocol::GetInstance()->CreateMsgId(p2pmessage);

        SocketMessage msg;
        msg.SetSocketfd(sfd);
        msg.SetP2PMessage(p2pmessage);
        // std::string payload = GetSerializedString(msg);
        // msg.SetPayload(payload);

        send_SocketMessage(sfd, msg);
    }

    return;
}
