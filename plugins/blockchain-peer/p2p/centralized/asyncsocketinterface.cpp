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

#include "peerlistmanager_combined.h"
#include "asyncsocketinterface.h"
#include "centralizedmsgproxy_async.h"
#include "../../blockchain/txpool.h"
#include "../../consensus/simpleconsensus.h"
#include "../../consensus/powconsensus_centralized_async.h"
#include "../../util/globalclock.h"
#include "../../util/nodeinfo.h"


#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/device/array.hpp>

AsyncSocketInterface* AsyncSocketInterface::instance = 0;

AsyncSocketInterface* AsyncSocketInterface::GetInstance() {
    if (instance == 0) {
        instance = new AsyncSocketInterface();
    }
    return instance;
}

// This callback is called when data is readable on the socket.
void AsyncSocketInterface::ServerCallback(EV_P_ ev_io *w, int revents) {
  puts("inet stream socket has become readable");

  struct 	sockaddr_in 	their_addr; /* connector's address information */
  int receiver_fd;
  socklen_t 			sin_size;
  // struct sock_ev_client* client;

  // since ev_io is the first member,
  // watcher `w` has the address of the 
  // start of the sock_ev_serv struct
  struct sock_ev_serv* server = (struct sock_ev_serv*) w;

  while (1)
  {
      sin_size = sizeof(struct sockaddr_in);
      receiver_fd = accept(server->fd, (struct sockaddr *)&their_addr, &sin_size);
      if( receiver_fd == -1 ) {
          if( errno != EAGAIN && errno != EWOULDBLOCK ) {
              cout << "accept() failed errno=" << errno << strerror(errno) << "\n";
              exit(EXIT_FAILURE);
          }
          break;
      }
      cout << "server: got connection from " << inet_ntoa(their_addr.sin_addr) << "\n"; 

      fcntl(receiver_fd, F_SETFL, O_NONBLOCK);

      PeerCombined* peer = new PeerCombined();
      peer->SetReceiverSocket(receiver_fd);
      peer->recv_status = RECV_IDLE;
      peer->ipaddr = std::string(inet_ntoa(their_addr.sin_addr));
      // peer->hostname = PeerListManagerCombined::GetInstance()->GetDomainFromIp(peer->ipaddr);
      cout << "server: got connection from (" << peer->ipaddr << "," << peer->hostname << ")" << "\n";
      PeerListManagerCombined::GetInstance()->GetPeerList().push_back(peer);

      sock_ev_receiver* receiver = new sock_ev_receiver();
      receiver->fd = receiver_fd;
      receiver->peer = peer;
      peer->ev_receiver = receiver;
      ev_io_init(&receiver->io, ReceiverCallback, receiver->fd, EV_READ);
      ev_io_start(EV_A_ &receiver->io);


      // create outgoing socket
      // if (peer->hostname != "") {
      //     int cli_sockfd;  

      //     if ((cli_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      //         perror("socket");
      //         exit(1);
      //     }

      //     peer->sender_sfd = cli_sockfd;

      //     struct sockaddr_in servaddr;
      //     bzero(&servaddr, sizeof(servaddr));
      //     servaddr.sin_family = AF_INET;
      //     servaddr.sin_port = htons(MYPORT);
      //     servaddr.sin_addr.s_addr = their_addr.sin_addr.s_addr;
      
      //     if (connect(peer->sender_sfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
      //         perror("connect");
      //         exit(1);
      //     }

      //     sock_ev_sender* sender = new sock_ev_sender();
      //     sender->fd = peer->sender_sfd;
      //     sender->peer = peer;
      //     ev_io_init(&sender->io, SenderCallback, sender->fd, EV_WRITE); // watcher is initialized but not enabled for catching write events


      //     peer->ev_sender = sender;

      // }
  }

  int processingType = AsyncSocketInterface::GetInstance()->GetProcessingQueueType();
  if (processingType == PROCESSINGQUEUETYPE_PROXY) {
      bool processed = true;
      while (processed) {
          processed = false;
          processed |= CentralizedMessageProxyAsync::GetInstance()->ProcessQueue();
      }
  }
  else if (processingType == PROCESSINGQUEUETYPE_NODE) {
      bool processed = true;
      while (processed) {
          processed = false;
          processed |= TxPool::GetInstance()->ProcessQueue();
          processed |= POWConsensus::GetInstance()->ProcessQueue();
          POWConsensus::GetInstance()->TriggerMiningEmulation();
      }
  }
}

// This callback is called when client data is available
void AsyncSocketInterface::ReceiverCallback(EV_P_ ev_io *w, int revents) {
  // a client has become readable

  sock_ev_receiver* receiver = (sock_ev_receiver*) w;
  PeerCombined *p = receiver->peer;
  char string_read[2000];
  int n;
  std::string domain = receiver->peer->hostname;

  if (revents & EV_READ) {
      std::cout << "read event" << "\n";
      switch (p->recv_status) {
      case RECV_IDLE:
          {
              int length = 0;
              n = recv(p->receiver_sfd,&length,sizeof(int),0);
              if (n == -1 && errno != EAGAIN){ 
                  perror("recv - non blocking \n");
                  cout << "errno=" << errno << "\n";
                  exit(1);
              }
              else if (n == 0) {
                  cout << "socket disconnected" << "\n";
                  ev_io_stop(EV_A_ &receiver->io);
                  close(receiver->fd);
                  delete receiver;
              }
              else if (n > 0) {
                  p->payload_len = length;
                  p->recv_status = RECV_LENGTH;
                  p->received_len = 0;
                  p->recv_str = "";
                  cout << "receive from " << p->hostname << ", network packet length:" << length << "\n";
                  // string_read[n] = '\0';
                  // cout << "The string is: " << string_read << "\n";
              }
              break;
          }
      case RECV_LENGTH:
          int total_recv_size = p->received_len;
          int numbytes = 0;

          while(1) {
              int recv_size = std::min(2000, p->payload_len - total_recv_size);
              numbytes = recv(p->receiver_sfd, string_read, recv_size, 0);              
              if (numbytes > 0) {
                  total_recv_size += numbytes;
                  p->recv_str.append(string_read, numbytes);
              }
              else if (numbytes == 0) {
                  std::cerr << "recv event: connection closed\n";
                  break;
              }
              else if (numbytes < 0) {
                  std::cerr << "recv event: recv payload fail\n";
                  break;
              }
              // std::cout << "recv:length=[" << numbytes << "],data=[" << recv_str << "]\n";
              if (total_recv_size == p->payload_len)
                  break;
              else {
                  std::cout << "recv: total_recv_size=" << total_recv_size << ", payload_len=" << p->payload_len << "\n";
              }
              memset(string_read, 0, 2000);
          }

          if (p->payload_len != total_recv_size) {
              p->received_len = total_recv_size;
              std::cout << utility::GetGlobalClock() << ":error: received only part of payload (maybe recv buffer is full)" << "received_len:" << p->received_len << ", payload_len:" << p->payload_len << ", from:" << p->hostname << "\n";
              break;
          }
          else {
              std::cout << "fully received payload. size:" << total_recv_size << ", from:" << p->hostname << "\n";
          }
          p->recv_status = RECV_IDLE;
          CentralizedNetworkMessage nmsg = GetDeserializedMsg(p->recv_str);
          if (nmsg.type == CentralizedNetworkMessage_DOMAINNOTIFYMSG) {
              Domain domain = boost::get<Domain>(nmsg.data);
              p->hostname = domain;
          }
          else if (nmsg.type == CentralizedNetworkMessage_BROADCASTREQMSG) {
              BroadcastRequestMessage pmsg = boost::get<BroadcastRequestMessage>(nmsg.data);
              CentralizedMessageProxyAsync::GetInstance()->PushToQueue(pmsg);
          }
          else if (nmsg.type == CentralizedNetworkMessage_UNICASTREQMSG) {
              UnicastRequestMessage umsg = boost::get<UnicastRequestMessage>(nmsg.data);
              CentralizedMessageProxyAsync::GetInstance()->PushToQueue(umsg);
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

  if (revents & EV_WRITE) {
      std::map< std::string, std::list<WriteMsg *> > &msgQueueMap = AsyncSocketInterface::GetInstance()->GetMsgQueueMap();
      if (msgQueueMap[domain].empty()) {
          ev_io_stop(loop, &receiver->io); 
          ev_io_set(&receiver->io, receiver->fd, EV_READ); // turn off write event 
          ev_io_start(loop, &receiver->io); 
          // ev_io_set(&sender->io, sender->fd, EV_READ); 
          return;
      } 
        
      WriteMsg* msg = msgQueueMap[domain].front();
      int numbytes = send(receiver->fd, msg->dpos(), msg->nbytes(), 0);
      if (numbytes < 0) {
          perror("write error");
          exit(-1);
      }
        
      msg->pos += numbytes;
      if (msg->nbytes() == 0) {
          msgQueueMap[domain].pop_front();
          delete msg;
      }
  }


  int processingType = AsyncSocketInterface::GetInstance()->GetProcessingQueueType();
  if (processingType == PROCESSINGQUEUETYPE_PROXY) {
      bool processed = true;
      while (processed) {
          processed = false;
          processed |= CentralizedMessageProxyAsync::GetInstance()->ProcessQueue();
      }
  }
  else if (processingType == PROCESSINGQUEUETYPE_NODE) {
      bool processed = true;
      while (processed) {
          processed = false;
          processed |= TxPool::GetInstance()->ProcessQueue();
          processed |= POWConsensus::GetInstance()->ProcessQueue();
          POWConsensus::GetInstance()->TriggerMiningEmulation();
      }
  }
}

// void AsyncSocketInterface::SenderCallback(EV_P_ ev_io *w, int revents) {
//     std::cout << "sender callback is called" << "\n";

//     sock_ev_sender* sender = (sock_ev_sender*) w; 
//     std::string domain = sender->peer->hostname;

    
//     if (revents & EV_READ) {
//         std::cout << "read possible but nothing to read" << "\n";
//     }

//     if (revents & EV_WRITE) {
//         std::map< std::string, std::list<WriteMsg *> > &msgQueueMap = AsyncSocketInterface::GetInstance()->GetMsgQueueMap();
//         if (msgQueueMap[domain].empty()) {
//             ev_io_stop(loop, &sender->io); // turn off write event watcher      
//             // ev_io_set(&sender->io, sender->fd, EV_READ); 
//             return;
//         } 
        
//         WriteMsg* msg = msgQueueMap[domain].front();
//         int numbytes = send(sender->fd, msg->dpos(), msg->nbytes(), 0);
//         if (numbytes < 0) {
//             perror("write error");
//             exit(-1);
//         }
        
//         msg->pos += numbytes;
//         if (msg->nbytes() == 0) {
//             msgQueueMap[domain].pop_front();
//             delete msg;
//         }
//     }
// }

void AsyncSocketInterface::ConnectCallback(EV_P_ ev_periodic *w, int revents) {
    std::cout << "Periodic connect callback is called" << "\n";

    periodic_ev_connect* ev_connect = (periodic_ev_connect*) w; 
    std::vector<std::pair<Domain, int> >::iterator it = ev_connect->domain_sfd_pairs.begin();
    
    int n;
    if (ev_connect->domain_sfd_pairs.empty()) {
        ev_periodic_stop(EV_A_ &ev_connect->periodic);
        return;
    }

    while (it != ev_connect->domain_sfd_pairs.end()) {
        std::cout << "while" << "\n";
        Domain domain = (*it).first;
        if (PeerListManagerCombined::GetInstance()->GetPeerByDomain(domain)) {
            // if PeerListManager already contains the peer with the domain, 
            // then close socket and remove the pair from pair list
            // and continue
            close((*it).second);
            it = ev_connect->domain_sfd_pairs.erase(it);
            continue;
        }
            
        int sfd = (*it).second;

        struct addrinfo* servinfo;
        n = getaddrinfo(domain.c_str(), NULL, NULL, &servinfo);
        if (n != 0) {
            cout << "error in connection : getaddrinfo" << "\n";
            continue;
        }

        struct sockaddr_in servaddr;
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(MYPORT);
        servaddr.sin_addr.s_addr = ((struct sockaddr_in*) (servinfo->ai_addr))->sin_addr.s_addr;

        std::cout << "before connect" << "\n";
        n = connect(sfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
        if (n < 0 && errno != EINPROGRESS) {
            perror("connect");
            exit(1);
        }
        else if (n == 0) {
            std::cout << "connection established" << "\n";
            PeerCombined* peer = new PeerCombined();
            peer->SetReceiverSocket(sfd);
            peer->recv_status = RECV_IDLE;
            peer->ipaddr = std::string(inet_ntoa(servaddr.sin_addr));
            peer->hostname = domain;
            cout << "Connected to (" << peer->ipaddr << "," << peer->hostname << ")" << "\n";
            PeerListManagerCombined::GetInstance()->GetPeerList().push_back(peer);

            sock_ev_receiver* receiver = new sock_ev_receiver();
            receiver->fd = sfd;
            receiver->peer = peer;
            peer->ev_receiver = receiver;
            ev_io_init(&receiver->io, ReceiverCallback, receiver->fd, EV_READ);
            ev_io_start(EV_A_ &receiver->io);

            CentralizedNetworkMessage cmsg(CentralizedNetworkMessage_DOMAINNOTIFYMSG, NodeInfo::GetInstance()->GetHostId());
            AsyncSocketInterface::GetInstance()->SendNetworkMsg(cmsg, peer->hostname);

            it = ev_connect->domain_sfd_pairs.erase(it);
            continue;
        }
        else {
            fd_set rset, wset;
            // struct timeval tval;
            FD_ZERO(&rset);
            FD_SET(sfd, &rset);
            wset = rset;
            // tval.tv_sec = 0;
            // tval.tv_usec = 0;
            
            n = select(sfd+1, &rset, &wset, NULL, NULL);
            if (n >= 1) {
                if (FD_ISSET(sfd, &rset) || FD_ISSET(sfd, &wset)) {
                    int error = 0;
                    socklen_t len = sizeof(error);
                    if (getsockopt(sfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
                        perror("getsockopt");
                        exit(-1);
                    }

                    if (error) {
                        cout << "connection failed" << "\n";
                        exit(-1);
                    }
                    else {
                        cout << "connection established" << "\n";

                        PeerCombined* peer = new PeerCombined();
                        peer->SetReceiverSocket(sfd);
                        peer->recv_status = RECV_IDLE;
                        peer->ipaddr = std::string(inet_ntoa(servaddr.sin_addr));
                        peer->hostname = domain;
                        cout << "Connected to (" << peer->ipaddr << "," << peer->hostname << ")" << "\n";
                        PeerListManagerCombined::GetInstance()->GetPeerList().push_back(peer);

                        sock_ev_receiver* receiver = new sock_ev_receiver();
                        receiver->fd = sfd;
                        receiver->peer = peer;
                        peer->ev_receiver = receiver;
                        ev_io_init(&receiver->io, ReceiverCallback, receiver->fd, EV_READ);
                        ev_io_start(EV_A_ &receiver->io);

                        CentralizedNetworkMessage cmsg(CentralizedNetworkMessage_DOMAINNOTIFYMSG, NodeInfo::GetInstance()->GetHostId());
                        AsyncSocketInterface::GetInstance()->SendNetworkMsg(cmsg, peer->hostname);

                        it = ev_connect->domain_sfd_pairs.erase(it);
                        continue;
                    }
                }
            }
            else {
                std::cout << "n=" << n << "\n";
                it++;
                continue;
            }
        }
    }
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

void AsyncSocketInterface::InitServerSocket() {
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

/**
 * Initialize the non-blocking socket for listening socket and client socket.
 */
void AsyncSocketInterface::InitializeListenSocket() {
    InitServerSocket();
}

void AsyncSocketInterface::RegisterPeriodicConnectWatcher(int num_dest, char *domain_dest[]) {
    periodic_ev_connect* ev_connect = new periodic_ev_connect();

    for (int i = 0; i < num_dest; i++) {
        std::cout << "Add target destination domain : " << domain_dest[i] << "\n";

        int cli_sockfd;
        if ((cli_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("client socket");
            exit(1);
        }        
        int flags = fcntl(cli_sockfd, F_GETFL, 0);
        fcntl(cli_sockfd, F_SETFL, flags | O_NONBLOCK); /* Change the socket into non-blocking state	*/

        ev_connect->domain_sfd_pairs.push_back(make_pair(std::string(domain_dest[i]), cli_sockfd));
    }

    ev_periodic_init(&ev_connect->periodic, ConnectCallback, 0, 0.01, 0);
    ev_periodic_start(EV_A_ &ev_connect->periodic);
}

void AsyncSocketInterface::RegisterServerWatcher() {
    // Adding server i/o event watcher
    server = new sock_ev_serv();
    server->fd = GetListenSocket();
    ev_io_init(&server->io, ServerCallback, server->fd, EV_READ);
    ev_io_start(EV_A_ &server->io);
}

void AsyncSocketInterface::SetEvLoop(struct ev_loop* l) {
    // Create our single-loop for this single-thread application    
    loop = l;
}

void AsyncSocketInterface::SendNetworkMsg(CentralizedNetworkMessage& msg, std::string dest) {
    PeerCombined *p = PeerListManagerCombined::GetInstance()->GetPeerByDomain(dest);
    if (p && p->ev_receiver) {
        std::string payload = GetSerializedString(msg);
        int payload_len = payload.size();
        std::cout << "SendNetworkMsg:" << "send to " << dest << ", payload size=" << payload_len << "\n";

        msgQueueMap[dest].push_back(new WriteMsg((char*)&payload_len, sizeof(int)));
        msgQueueMap[dest].push_back(new WriteMsg(payload.c_str(), payload_len));

        ev_io_stop(loop, &p->ev_receiver->io); 
        ev_io_set(&p->ev_receiver->io, p->ev_receiver->fd, EV_READ | EV_WRITE); // turn on write event 
        ev_io_start(loop, &p->ev_receiver->io); 

        // ev_io_start(loop, &p->ev_sender->io);
    }
    else {
        std::cout << "no valid peer or outgoing socket exists for " << dest << "\n";
    }
    return;
}
