#include "HandleNetwork_ProxyModel.h"
#include "../event/GlobalEvent.h"
#include "../Configuration.h" 

#include <fcntl.h> /* Added for the nonblocking socket */
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <iostream>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/serialization/export.hpp>


#include "TestMessage.h"
BOOST_CLASS_EXPORT(TestMessage);

int HandleNetwork_ProxyModel::InitializeListenSocket() {
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

    // serverListenSocket = sockfd;
    return sockfd;
}

void HandleNetwork_ProxyModel::RegisterServerWatcher(int listenfd) {
    SocketEventWatcher *watcher = new SocketEventWatcher(listenfd);
    watcher->InitEventWatcher(GlobalEvent::onAcceptSocketIO, EV_READ);
    watcher->StartEventWatcher();
    GlobalEvent::socketWatcherMap.insert( std::pair<int, SocketEventWatcher*>(listenfd, watcher));
}

void HandleNetwork_ProxyModel::RegisterSocketWatcher(int sfd) {
    SocketEventWatcher *watcher = new SocketEventWatcher(sfd);
    watcher->InitEventWatcher(GlobalEvent::onSendRecvSocketIO, EV_READ);
    watcher->StartEventWatcher();
    GlobalEvent::socketWatcherMap.insert( std::pair<int, SocketEventWatcher*>(sfd, watcher));
}

int HandleNetwork_ProxyModel::ConnectToNode(std::string domain) {
    // if membershipPeerList already contains the peer for given domain, 
    // then we dont need to do anything here, so return 0.
    if (membershipPeerList.GetPeerByDomain(domain)) {
        return 0;
    }

    int sfd;
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("client socket");
        return -1;
    }        
    int flags = fcntl(sfd, F_GETFL, 0);
    fcntl(sfd, F_SETFL, flags | O_NONBLOCK); /* Change the socket into non-blocking state	*/
    
    struct addrinfo* servinfo;
    int n = getaddrinfo(domain.c_str(), NULL, NULL, &servinfo);
    if (n != 0) {
        std::cout << "error in connection : getaddrinfo" << "\n";
        return -1;
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);
    servaddr.sin_addr.s_addr = ((struct sockaddr_in*) (servinfo->ai_addr))->sin_addr.s_addr;

    n = connect(sfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if (n < 0 && errno != EINPROGRESS) {
        // perror("connect");
        // exit(1);
        perror("connect");
        std::cout << "Unable to connect to " << domain << "\n";
        return -1;
    }
    else if (n == 0) {
        std::cout << "connection established" << "\n";
        Peer* peer = new Peer(sfd, domain, std::string(inet_ntoa(servaddr.sin_addr)));
        peerMap[sfd] = peer;
        std::cout << "Connected to (" << peer->GetIP() << "," << peer->GetHostname() << ")" << "\n";
        // This simple module maintains the same membership & gossip peerlist.
        membershipPeerList.AppendPeer(peer);
        gossipPeerList.AppendPeer(peer);     
        RegisterSocketWatcher(sfd);

        Message* msg = new TestMessage("I don't know my domain name.");
        std::cout << "allocated" << "\n";
        UnicastMsg(peer->GetIP(), msg);
        delete msg;
    }
    else { 
        // Now in progress of connection.
        // So wait for completion.
        fd_set rset, wset;        
        FD_ZERO(&rset);
        FD_SET(sfd, &rset);
        wset = rset;

        n = select(sfd+1, &rset, &wset, NULL, NULL);
        if (n >= 1) {
            if (FD_ISSET(sfd, &rset) || FD_ISSET(sfd, &wset)) {
                int error = 0;
                socklen_t len = sizeof(error);
                if (getsockopt(sfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
                    perror("getsockopt");
                    return -1;
                }

                if (error) {
                    std::cout << "connection failed" << "\n";
                    return -1;
                }
                else {
                    std::cout << "connection established" << "\n";
                    Peer* peer = new Peer(sfd, domain, std::string(inet_ntoa(servaddr.sin_addr)));
                    peerMap[sfd] = peer;
                    std::cout << "Connected to (" << peer->GetIP() << "," << peer->GetHostname() << ")" << "\n";
                    // This simple module maintains the same membership & gossip peerlist.
                    membershipPeerList.AppendPeer(peer);
                    gossipPeerList.AppendPeer(peer);     
                    RegisterSocketWatcher(sfd);

                    Message *msg = new TestMessage("I don't know my domain name.");
                    std::cout << "allocated" << "\n";
                    UnicastMsg(peer->GetIP(), msg);
                    delete msg;
                }
            }
        }
        else {
            std::cout << "Something wrong while connection : n=" << n << "\n";
            return -1;
        }
    }

    return 0;
}

int HandleNetwork_ProxyModel::JoinNetwork() {
    // Initialize a listening socket
    int listenfd = InitializeListenSocket();
    // Register a watcher which monitors the new connection requested from outside 
    RegisterServerWatcher(listenfd);

    if (amIProxyNode)
        return 0;

    // Join P2P network by connecting to hardcoded node called "bleep0"
    // Connect to peer and register I/O watcher
    int result = ConnectToNode("bleep0");
    return result;
}

void HandleNetwork_ProxyModel::HandleRecvSocketIO(int fd) {
    auto it = peerMap.find(fd);
    if (it == peerMap.end()) {
        std::cout << "No valid peer exists" << "\n";
        exit(-1);
    }
    
    std::cout << "handle recv" << "\n";

    Peer* peer = it->second;
    SocketEventStatus &status = peer->GetSocketEventStatus();
    SocketEventWatcher *watcher = GlobalEvent::socketWatcherMap[fd];

    char string_read[2000];
    int n;
    switch (status.recv_status) {
    case RECV_IDLE:
        {
            int length = 0;
            n = recv(fd,&length,sizeof(int),0);
            if (n == -1 && errno != EAGAIN){ 
                perror("recv - non blocking \n");
                std::cout << "errno=" << errno << "\n";
                exit(-1);
            }
            else if (n == 0) {
                std::cout << "socket disconnected" << "\n";
                watcher->StopEventWatcher();
                membershipPeerList.RemovePeer(peer);
                gossipPeerList.RemovePeer(peer);
                auto watcher_it = GlobalEvent::socketWatcherMap.find(fd);
                GlobalEvent::socketWatcherMap.erase(watcher_it);
                auto peer_it = peerMap.find(fd);
                peerMap.erase(peer_it);
                delete peer;
                delete watcher;
                close(fd);
            }
            else if (n > 0) {
                status.payload_len = length;
                status.recv_status = RECV_LENGTH;
                status.received_len = 0;
                status.recv_str = "";
                std::cout << "receive from " << peer->GetIP() << ", network packet length:" << length << "\n";
                // string_read[n] = '\0';
                // cout << "The string is: " << string_read << "\n";
            }
            break;
        }
    case RECV_LENGTH:
        {
            int total_recv_size = status.received_len;
            int numbytes = 0;

            while(1) {
                int recv_size = std::min(2000, status.payload_len - total_recv_size);
                numbytes = recv(fd, string_read, recv_size, 0);              
                if (numbytes > 0) {
                    total_recv_size += numbytes;
                    status.recv_str.append(string_read, numbytes);
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
                if (total_recv_size == status.payload_len)
                    break;
                else {
                    std::cout << "recv: total_recv_size=" << total_recv_size << ", payload_len=" << status.payload_len << "\n";
                }
                memset(string_read, 0, 2000);
            }

            if (status.payload_len != total_recv_size) {
                status.received_len = total_recv_size;
                std::cout << "error: received only part of payload (maybe recv buffer is full)" << "received_len:" << status.received_len << ", payload_len:" << status.payload_len << ", from:" << peer->GetIP() << "\n";
                break;
            }
            else {
                std::cout << "fully received payload. size:" << total_recv_size << ", from:" << peer->GetIP() << "\n";
            }
            status.recv_status = RECV_IDLE;

            Message *msg = GetDeserializedMsg(status.recv_str);
            switch (msg->GetType()) {
            case Message::TEST_MESSAGE: 
                {
                    std::cout << "Deserialization of the received message complete!" << "\n";
                    std::cout << "MESSAGE TYPE = TEST MESSAGE" << "\n";
                    delete msg;
                    break;
                }
            case Message::TX_MESSAGE:
                {
                    std::cout << "Deserialization of the received message complete!" << "\n";
                    std::cout << "MESSAGE TYPE = TX MESSAGE" << "\n";
                    delete msg;
                    break;
                }
            }

            // fill here with parsing logic
            // ...
            // ...
            // CentralizedNetworkMessage nmsg = GetDeserializedMsg(p->recv_str);
            break;
        }
    }

    return;
}

void HandleNetwork_ProxyModel::HandleSendSocketIO(int fd) {
    auto it = peerMap.find(fd);
    if (it == peerMap.end()) {
        std::cout << "No valid peer exists" << "\n";
        exit(-1);
    }
    
    Peer* peer = it->second;
    SocketEventStatus &status = peer->GetSocketEventStatus();
    SocketEventWatcher *watcher = GlobalEvent::socketWatcherMap[fd];

    if (status.sendMsgQueue.empty()) {
        watcher->StopEventWatcher();
        watcher->SetEventWatcher(EV_READ); // turn off write event 
        watcher->StartEventWatcher();
        return;
    } 
        
    WriteMsg* msg = status.sendMsgQueue.front();
    int numbytes = send(fd, msg->dpos(), msg->nbytes(), 0);
    if (numbytes < 0) {
        perror("write error");
        exit(-1);
    }
        
    msg->pos += numbytes;
    if (msg->nbytes() == 0) {
        status.sendMsgQueue.pop_front();
        delete msg;
    }
}

void HandleNetwork_ProxyModel::HandleAcceptSocketIO(int fd) {
    struct 	sockaddr_in 	their_addr; /* connector's address information */
    int sock_fd;
    socklen_t 			sin_size;

    // Handle pending 'accept' until there's no remaining accept requests.
    while (1)
        {
            sin_size = sizeof(struct sockaddr_in);
            sock_fd = accept(fd, (struct sockaddr *)&their_addr, &sin_size);
            if( sock_fd == -1 ) {
                if( errno != EAGAIN && errno != EWOULDBLOCK ) {
                    std::cout << "accept() failed errno=" << errno << strerror(errno) << "\n";
                    exit(EXIT_FAILURE);
                }
                break;
            }
            std::cout << "server: got connection from " << inet_ntoa(their_addr.sin_addr) << "\n"; 

            fcntl(sock_fd, F_SETFL, O_NONBLOCK);

            Peer* peer = new Peer(sock_fd, std::string(inet_ntoa(their_addr.sin_addr)));
            peerMap[sock_fd] = peer;
            std::cout << "server: got connection from (" << peer->GetIP() << "," << peer->GetHostname() << ")" << "\n";

            // This simple module maintains the same membership & gossip peerlist.
            membershipPeerList.AppendPeer(peer);
            gossipPeerList.AppendPeer(peer);     

            // Register a watcher which monitors any (recv || send) for given socket
            RegisterSocketWatcher(sock_fd);
        }
    return;
}

std::string HandleNetwork_ProxyModel::GetSerializedString(Message* msg) {
  std::string serial_str;
  // serialize obj into an std::string payload
  boost::iostreams::back_insert_device<std::string> inserter(serial_str);
  boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
  boost::archive::binary_oarchive oa(s);
  oa << msg;
  s.flush();
  return serial_str;
}

Message* HandleNetwork_ProxyModel::GetDeserializedMsg(std::string str) {
  Message* msg;
  // wrap buffer inside a stream and deserialize string_read into obj
  boost::iostreams::basic_array_source<char> device(str.c_str(), str.size());
  boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
  boost::archive::binary_iarchive ia(s);
  ia >> msg;
  return msg;
}

void HandleNetwork_ProxyModel::UnicastMsg(std::string destip, Message* msg) {
    Peer* p = membershipPeerList.GetPeerByIP(destip);
    if (p) {
        std::string payload = GetSerializedString(msg);
        int payload_len = payload.size();
        std::cout << "UnicastMsg:" << "send to " << destip << ", payload size=" << payload_len << "\n";
        
        SocketEventStatus &status = p->GetSocketEventStatus();
        status.sendMsgQueue.push_back(new WriteMsg((char*)&payload_len, sizeof(int)));
        status.sendMsgQueue.push_back(new WriteMsg(payload.c_str(), payload_len));

        SocketEventWatcher *watcher = GlobalEvent::socketWatcherMap[p->GetSocketFD()];        
        watcher->StopEventWatcher();
        watcher->SetEventWatcher(EV_READ | EV_WRITE); // turn on write event 
        watcher->StartEventWatcher();
    }
    else {
        std::cout << "no valid peer or outgoing socket exists for " << destip << "\n";
    }
    return;
}
