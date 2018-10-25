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

#include "peerlistmanager.h"
#include "simplesocketinterface.h"
#include "centralizedmsgproxy.h"
#include "../../blockchain/txpool.h"
#include "../../consensus/simpleconsensus.h"
#include "../../consensus/powconsensus.h"


#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/device/array.hpp>

SimpleSocketInterface* SimpleSocketInterface::instance = 0;

SimpleSocketInterface* SimpleSocketInterface::GetInstance() {
    if (instance == 0) {
        instance = new SimpleSocketInterface();
    }
    return instance;
}

std::string GetSerializedString(CentralizedNetworkMessage msg) {
  std::string serial_str;
  // serialize obj into an std::string payload
  boost::iostreams::back_insert_device<std::string> inserter(serial_str);
  boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
  boost::archive::binary_oarchive oa(s);
  oa << msg;
  s.flush();
  return serial_str;
}

CentralizedNetworkMessage GetDeserializedMsg(std::string str) {
  CentralizedNetworkMessage msg;
  // wrap buffer inside a stream and deserialize string_read into obj
  boost::iostreams::basic_array_source<char> device(str.c_str(), str.size());
  boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
  boost::archive::binary_iarchive ia(s);
  ia >> msg;
  return msg;
}


void SimpleSocketInterface::InitServerSocket() {
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

void SimpleSocketInterface::InitClientSocket(PeerList& outPeerList) {

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
void SimpleSocketInterface::InitializeSocket(PeerList& outPeerList) {
    InitServerSocket();

    InitClientSocket(outPeerList);
}

/**
 * Process socket events
 * 1. process non-blocking accept (and make new InPeer)
 * 2. process non-blocking connect (try to connect a socket using IDLE OutPeerList and update them as CONNECTED if socket is connected)
 * 3. process non-blocking recv
 */
void SimpleSocketInterface::ProcessNonblockSocket(PeerList& inPeerList, PeerList& outPeerList) {
    struct 	sockaddr_in 	their_addr; /* connector's address information */
    int                         new_fd; /* new connection on new_fd */
    socklen_t 			sin_size;
    char			string_read[2000];
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

        Peer* inPeer = new Peer(new_fd);
        inPeer->recv_status = RECV_IDLE;
        inPeer->conn_status = NONE;
        SimplePeerListManager::GetInstance()->GetInPeerList().push_back(inPeer);
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
                // send_sfd_list.push_back(p->sfd);
                int flags = fcntl(p->sfd, F_GETFL, 0);
                fcntl(p->sfd, F_SETFL, flags & (~O_NONBLOCK)); /* Change the socket into blocking state	*/
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
                        // send_sfd_list.push_back(p->sfd);

                        int flags = fcntl(p->sfd, F_GETFL, 0);
                        fcntl(p->sfd, F_SETFL, flags & (~O_NONBLOCK)); /* Change the socket into blocking state	*/

                    }
                }
            }

            break;
        case CONNECTED:
            break;
        }
        
    }

    // 3. Process non-blocking recv
    for (PeerList::iterator it = inPeerList.begin(); it != inPeerList.end(); it++) {    
        Peer* p = *it;

        switch (p->recv_status) {
        case RECV_IDLE:
            {
                int length = 0;
                n = recv(p->sfd,&length,sizeof(int),0);
                if (n == -1 && errno != EAGAIN){ 
                    perror("recv - non blocking \n");
                    cout << "errno=" << errno << "\n";
                    exit(1);
                }
                else if (n == 0) {
                    cout << "socket disconnected" << "\n";
                }
                else if (n > 0) {
                    p->payload_len = length;
                    p->recv_status = RECV_LENGTH;
                    cout << "receive network packet length:" << length << "\n";
                    // string_read[n] = '\0';
                    // cout << "The string is: " << string_read << "\n";
                }
                break;
            }
        case RECV_LENGTH:
            int total_recv_size = 0;
            int numbytes = 0;
            std::string recv_str;

            while(1) {
              int recv_size = std::min(2000, p->payload_len - total_recv_size);
              numbytes = recv(p->sfd, string_read, recv_size, 0);              
              if (numbytes > 0) {
                total_recv_size += numbytes;
                recv_str.append(string_read, numbytes);
              }
              else if (numbytes == 0) {
                std::cerr << "recv event: connection closed\n";
                break;
              }
              else if (numbytes < 0) {
                if (errno != EWOULDBLOCK) {
                  std::cerr << "recv event: recv payload fail\n";
                  break;
                }
              }
              // std::cout << "recv:length=[" << numbytes << "],data=[" << recv_str << "]\n";
              if (total_recv_size == p->payload_len)
                break;
              memset(string_read, 0, 2000);
            }

            if (p->payload_len != total_recv_size) {
                std::cout << "error: received only part of payload" << "\n";
                exit(1);
            }

            cout << "received payload" << "\n";
            p->recv_status = RECV_IDLE;
            CentralizedNetworkMessage nmsg = GetDeserializedMsg(recv_str);
            if (nmsg.type == CentralizedNetworkMessage_BROADCASTREQMSG) {
                BroadcastRequestMessage pmsg = boost::get<BroadcastRequestMessage>(nmsg.data);
                CentralizedMessageProxy::GetInstance()->PushToQueue(pmsg);
            }
            else if (nmsg.type == CentralizedNetworkMessage_UNICASTREQMSG) {
                UnicastRequestMessage umsg = boost::get<UnicastRequestMessage>(nmsg.data);
                CentralizedMessageProxy::GetInstance()->PushToQueue(umsg);
            }
            else if (nmsg.type == CentralizedNetworkMessage_PROXYGENERATEDMSG) {
                ProxyGeneratedMessage msg = boost::get<ProxyGeneratedMessage>(nmsg.data);
                switch(msg.type) {
                case ProxyGeneratedMessage_TRANSACTION: 
                    {
                        Transaction tx = boost::get<Transaction>(msg.data);
                        TxPool::GetInstance()->PushTxToQueue(tx);
                        std::cerr << "recv new tx\n";
                    }
                    break;

                case ProxyGeneratedMessage_BLOCK:
                    {
                        std::cerr << "recv new blk\n";

                        // Must be propagated to proper ledgermanager.
                        // Currently, do thing.

                        Block *blk = boost::get<Block>(&msg.data);
                        if (blk) {
                            std::cout << "Following block is received" << "\n";
                            std::cout << *blk << "\n";
                        }
                        else {
                            std::cout << "Wrong data in P2PMessage" << "\n";
                            exit(1);
                        }
                    }
                    break;

                case ProxyGeneratedMessage_SIMPLECONSENSUSMESSAGE:
                    {
                        SimpleConsensusMessage cmsg = boost::get<SimpleConsensusMessage>(msg.data);
                        SimpleConsensus::GetInstance()->PushToQueue(cmsg);
                        std::cerr << "recv new css msg\n";
                    }
                    break;
                case ProxyGeneratedMessage_POWCONSENSUSMESSAGE:
                    {
                        POWConsensusMessage powmsg = boost::get<POWConsensusMessage>(msg.data);
                        POWConsensus::GetInstance()->PushToQueue(powmsg);
                        std::cout << "pushed POWConsensusMessage (unicast msg) to queue" << "\n";
                    }
                    break;
                } 
            }
            break;
        }
    }
}

// For simplicity, I make it as a function, instead of a queue-based communication.
// TODO : Implement a queue for SimpleSocketInterface. 
// In other words, we need to unify the communication mechanism between modules. 
void SimpleSocketInterface::SendNetworkMsg(CentralizedNetworkMessage& msg, std::string dest) {
    int sfd = -1;
    PeerList& outPeerList = SimplePeerListManager::GetInstance()->GetOutPeerList();

    for (Peer* outPeer : outPeerList) {
        if (outPeer->conn_status == CONNECTED && outPeer->hostname == dest) {
            sfd = outPeer->sfd;
            break;
        }
    }
    
    if (sfd == -1) {
        std::cout << "no valid peer exists for " << dest << "\n";
        exit(-1);
    }
  
    std::cout << "SendNetworkMsg:" << "send to " << dest << "\n";
    std::string payload = GetSerializedString(msg);
    int payload_len = payload.size();
    std::cout << "SendNetworkMsg:" << "send to " << dest << ", payload size=" << payload_len << "\n";

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
    std::cout << "SendNetworkMsg:" << "send to " << dest << ", payload size=" << payload_len << " complete\n";
}


// void SimpleSocketInterface::ProcessQueue() {
//     while (!msgQueue.empty()) {
//         SocketMessage msg = msgQueue.front();
        
//         // send to blocking socket
//         for (vector<int>::iterator it = send_sfd_list.begin(); it != send_sfd_list.end(); it++) {
//             int sfd = *it;

//             int payload_length = msg.GetPayloadLength();
//             int n = send(sfd, (char*)&payload_length, sizeof(int), 0);
//             if (n < 0){ 
//                 cout << "send errno=" << errno << "\n";
//                 exit(1);
//             }
//             else if (n < sizeof(int)) {
//                 cout << "Warning : sented string is less than requested" << "\n";
//                 cout << "sented string length: " << n << "\n";
//                 exit(1);
//             }
//             else {
//                 cout << "sented string length: " << n << "\n";
//             }            
            
//             n = send(sfd,msg.GetPayload().c_str(),payload_length,0);
//             if (n < 0){ 
//                 cout << "send errno=" << errno << "\n";
//                 exit(1);
//             }
//             else if (n < payload_length) {
//                 cout << "Warning : sented string is less than requested" << "\n";
//                 cout << "sented string length: " << n << "\n";
//                 exit(1);
//             }
//             else {
//                 cout << "sented string length: " << n << "\n";
//             }            
//         }

//         msgQueue.pop();
//     }
// }
