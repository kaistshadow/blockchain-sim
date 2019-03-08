#include "HandleNetwork_ProxyModel.h"
#include "../event/GlobalEvent.h"
#include "../Configuration.h"
#include "../utility/NodeInfo.h"
#include "../utility/GlobalClock.h"

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

#include "../datamodules/Transaction.h"
// BOOST_CLASS_EXPORT(Transaction);
BOOST_CLASS_EXPORT(SimpleTransaction);
BOOST_CLASS_EXPORT(UselessTransaction);

#include "../consensus/POWConsensusMessage.h"
BOOST_CLASS_EXPORT(POWConsensusMessage);

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
        socketEventPublisher.RegisterSocketAsDataSocket(sfd);

        // PrintGossipPeerList();//test

        // Message* msg = new TestMessage("I don't know my domain name.");
        // Transaction* msg = new Transaction(10,15,1.2);
        // std::cout << "allocated tx" << "\n";
        // UnicastMsg(peer->GetIP(), msg);
        // delete msg;
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

                    // PrintGossipPeerList();//test

                    socketEventPublisher.RegisterSocketAsDataSocket(sfd);
                    // Message* msg = new TestMessage("I don't know my domain name.");
                    // Transaction* msg = new Transaction(10,15,1.2);
                    // std::cout << "allocated tx" << "\n";
                    // if (NodeInfo::GetInstance()->GetHostIP() == "11.0.0.4")
                    //     BroadcastMsg(msg);
                    //     UnicastMsg("11.0.0.3", msg);
                    // else
                    //     UnicastMsg(peer->GetIP(), msg);
                    // delete msg;
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

void HandleNetwork_ProxyModel::RelayUnicastMsg(MessageHeader* header, Message* msg) {
    Peer* p = membershipPeerList.GetPeerByIP(header->GetDestIP());
    if (!p) {
        // no valid peer or outgoing socket exists for destination
        // in proxy model, we leverage hardcoded proxy whose domain is bleep0
        p = gossipPeerList.GetPeerByDomain("bleep0");
        if (!p) {
            std::cout << "no valid peer or outgoing socket exists for " << header->GetDestIP() << "\n";
            exit(-1);
        }
    }

    std::string message = GetSerializedString(msg);
    int message_len = message.size();

    std::string message_header = GetSerializedString(header);
    int header_len = message_header.size();

    std::cout << "UnicastMsg:" << "relay to " << p->GetIP() << ", header size=" << header_len << ", message size=" << message_len << "\n";

    SocketSendBuffer& buff = sendBuffMap[p->GetSocketFD()];
    buff.sendMsgQueue.push_back(std::shared_ptr<WriteMsg>(new WriteMsg((char*)&header_len, sizeof(int))));
    buff.sendMsgQueue.push_back(std::shared_ptr<WriteMsg>(new WriteMsg(message_header.c_str(), header_len)));
    buff.sendMsgQueue.push_back(std::shared_ptr<WriteMsg>(new WriteMsg(message.c_str(), message_len)));

    socketEventPublisher.SetSocketWrite(p->GetSocketFD());
    return;
}

void HandleNetwork_ProxyModel::RelayBroadcastMsg(MessageHeader* header, Message* msg) {
    std::list<Peer*>& peerlist = gossipPeerList.GetPeerList();
    for (Peer* p : peerlist) {
        if (header->IsVisitedIP(p->GetIP()))
            continue;

        std::string message = GetSerializedString(msg);
        int message_len = message.size();

        std::string message_header = GetSerializedString(header);
        int header_len = message_header.size();

        // std::cout << "BroadcastMsg:" << "relay to " << p->GetIP() << ", header size=" << header_len << ", message size=" << message_len << "\n";

        SocketSendBuffer& buff = sendBuffMap[p->GetSocketFD()];
        buff.sendMsgQueue.push_back(std::shared_ptr<WriteMsg>(new WriteMsg((char*)&header_len, sizeof(int))));
        buff.sendMsgQueue.push_back(std::shared_ptr<WriteMsg>(new WriteMsg(message_header.c_str(), header_len)));
        buff.sendMsgQueue.push_back(std::shared_ptr<WriteMsg>(new WriteMsg(message.c_str(), message_len)));

        socketEventPublisher.SetSocketWrite(p->GetSocketFD());
    }
}

std::string HandleNetwork_ProxyModel::GetSerializedString(Message* msg) {
  std::string serial_str;
  // serialize obj into an std::string message
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

std::string HandleNetwork_ProxyModel::GetSerializedString(MessageHeader* msg) {
  std::string serial_str;
  // serialize obj into an std::string message
  boost::iostreams::back_insert_device<std::string> inserter(serial_str);
  boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
  boost::archive::binary_oarchive oa(s);
  oa << msg;
  s.flush();
  return serial_str;
}

MessageHeader* HandleNetwork_ProxyModel::GetDeserializedMsgHeader(std::string str) {
  MessageHeader* msg;
  // wrap buffer inside a stream and deserialize string_read into obj
  boost::iostreams::basic_array_source<char> device(str.c_str(), str.size());
  boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
  boost::archive::binary_iarchive ia(s);
  ia >> msg;
  return msg;
}

int HandleNetwork_ProxyModel::JoinNetwork() {
    // Initialize a listening socket
    int listenfd = InitializeListenSocket();
    // Register a watcher which monitors the new connection requested from outside
    // RegisterServerWatcher(listenfd);
    socketEventPublisher.RegisterSocketAsServerSocket(listenfd);

    if (amIProxyNode)
        return 0;

    // Join P2P network by connecting to hardcoded node called "bleep0"
    // Connect to peer and register I/O watcher
    int result = ConnectToNode("bleep0");
    return result;
}


// message format
// | header_length || type || destip || visit_ip_list || message_length || message |
// | header_length ||                h   e  a   d   e  r                || message |
void HandleNetwork_ProxyModel::UnicastMsg(std::string destip, Message* msg) {
    Peer* p = membershipPeerList.GetPeerByIP(destip);
    if (!p) {
        // no valid peer or outgoing socket exists for destination
        // in proxy model, we leverage hardcoded proxy whose domain is bleep0
        p = gossipPeerList.GetPeerByDomain("bleep0");
        if (!p) {
            std::cout << "no valid peer or outgoing socket exists for " << destip << "\n";
            exit(-1);
        }
    }

    std::string message = GetSerializedString(msg);
    int message_len = message.size();

    MessageHeader header;
    header.SetBroadcastType(BROADCASTTYPE_UNICAST);
    header.SetDestIP(destip);
    header.SetMessageLength(message_len);

    std::string message_header = GetSerializedString(&header);
    int header_len = message_header.size();

    std::cout << "UnicastMsg:" << "send to " << destip << ", header size=" << header_len << ", message_size=" << message_len << "\n";

    SocketSendBuffer& buff = sendBuffMap[p->GetSocketFD()];
    buff.sendMsgQueue.push_back(std::shared_ptr<WriteMsg>(new WriteMsg((char*)&header_len, sizeof(int))));
    buff.sendMsgQueue.push_back(std::shared_ptr<WriteMsg>(new WriteMsg(message_header.c_str(), header_len)));
    buff.sendMsgQueue.push_back(std::shared_ptr<WriteMsg>(new WriteMsg(message.c_str(), message_len)));


    socketEventPublisher.SetSocketWrite(p->GetSocketFD());

    return;
}

void HandleNetwork_ProxyModel::BroadcastMsg(Message* msg) {
    std::list<Peer*>& peerlist = gossipPeerList.GetPeerList();
    for (Peer* p : peerlist) {
        std::string message = GetSerializedString(msg);
        int message_len = message.size();

        MessageHeader header;
        header.SetBroadcastType(BROADCASTTYPE_BROADCAST);
        header.AppendVisitedIP(NodeInfo::GetInstance()->GetHostIP());
        header.SetMessageLength(message_len);

        std::string message_header = GetSerializedString(&header);
        int header_len = message_header.size();

        // std::cout << "BroadcastMsg:" << "send to " << p->GetIP() << ", header size=" << header_len << ", message size=" << message_len << "\n";

        SocketSendBuffer& buff = sendBuffMap[p->GetSocketFD()];
        buff.sendMsgQueue.push_back(std::shared_ptr<WriteMsg>(new WriteMsg((char*)&header_len, sizeof(int))));
        buff.sendMsgQueue.push_back(std::shared_ptr<WriteMsg>(new WriteMsg(message_header.c_str(), header_len)));
        buff.sendMsgQueue.push_back(std::shared_ptr<WriteMsg>(new WriteMsg(message.c_str(), message_len)));

        socketEventPublisher.SetSocketWrite(p->GetSocketFD());
    }
}

void HandleNetwork_ProxyModel::onRecvSocketConnectionEvent(std::shared_ptr<EventInfo> info) {
    std::shared_ptr<RecvSocketConnectionEventInfo> derivedInfo = std::static_pointer_cast<RecvSocketConnectionEventInfo>(info);

    int fd = derivedInfo->GetListenFD();

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

            // PrintGossipPeerList();//test
            // Register a watcher which monitors any (recv || send) for given socket
            // RegisterSocketWatcher(sock_fd);
            socketEventPublisher.RegisterSocketAsDataSocket(sock_fd);
        }
    return;

}

void HandleNetwork_ProxyModel::onRecvSocketDataEvent(std::shared_ptr<EventInfo> info) {
    std::shared_ptr<RecvSocketDataEventInfo> derivedInfo = std::static_pointer_cast<RecvSocketDataEventInfo>(info);

    int fd = derivedInfo->GetSocketFD();

    auto it = peerMap.find(fd);
    if (it == peerMap.end()) {
        std::cout << "No valid peer exists" << "\n";
        exit(-1);
    }

    // std::cout << "handle recv" << "\n";

    Peer* peer = it->second;
    SocketRecvBuffer& buff = recvBuffMap[peer->GetSocketFD()];

    char string_read[2000];
    int n;
    switch (buff.recv_status) {
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
                socketEventPublisher.UnregisterDataSocket(fd);

                membershipPeerList.RemovePeer(peer);
                gossipPeerList.RemovePeer(peer);

                auto peer_it = peerMap.find(fd);
                peerMap.erase(peer_it);
                delete peer;
                close(fd);
            }
            else if (n > 0) {
                buff.message_len = length;
                buff.recv_status = RECV_HEADER;
                buff.received_len = 0;
                buff.recv_str = "";
                // std::cout << "receive from " << peer->GetIP() << ", header length:" << length << "\n";
                // string_read[n] = '\0';
                // cout << "The string is: " << string_read << "\n";
            }
            break;
        }
    case RECV_HEADER:
    case RECV_MSG:
        {
            int total_recv_size = buff.received_len;
            int numbytes = 0;

            while(1) {
                int recv_size = std::min(2000, buff.message_len - total_recv_size);
                numbytes = recv(fd, string_read, recv_size, 0);
                if (numbytes > 0) {
                    total_recv_size += numbytes;
                    buff.recv_str.append(string_read, numbytes);
                }
                else if (numbytes == 0) {
                    std::cerr << "recv event: connection closed\n";
                    break;
                }
                else if (numbytes < 0) {
                    std::cerr << "recv event: recv message fail\n";
                    break;
                }
                // std::cout << "recv:length=[" << numbytes << "],data=[" << recv_str << "]\n";
                if (total_recv_size == buff.message_len)
                    break;
                else {
                    std::cout << "recv: total_recv_size=" << total_recv_size << ", message_len=" << buff.message_len << "\n";
                }
                memset(string_read, 0, 2000);
            }

            if (buff.message_len != total_recv_size) {
                buff.received_len = total_recv_size;
                std::cout << "error: received only part of message (maybe recv buffer is full)" << "received_len:" << buff.received_len << ", message_len:" << buff.message_len << ", from:" << peer->GetIP() << "\n";
                break;
            }
            else {
                // std::cout << "fully received. size:" << total_recv_size << ", from:" << peer->GetIP() << "\n";
            }

            if (buff.recv_status == RECV_HEADER) {
                buff.header = GetDeserializedMsgHeader(buff.recv_str);
                buff.message_len = buff.header->GetMessageLength();
                buff.recv_status = RECV_MSG;
                buff.received_len = 0;
                buff.recv_str = "";
                break;
            }
            else if (buff.recv_status == RECV_MSG) {
                buff.recv_status = RECV_IDLE;
            }

            Message *msg = GetDeserializedMsg(buff.recv_str);

            // Now handler received entire header & msg
            if (buff.header->GetBroadcastType() == BROADCASTTYPE_UNICAST &&
                buff.header->GetDestIP() != NodeInfo::GetInstance()->GetHostIP() ) {
                // When msg is unicast message, and current node is not the destination.
                // So, relay the received unicast msg.
                RelayUnicastMsg(buff.header, msg);
                delete buff.header;
                delete msg;
                break;
            }
            else if (buff.header->GetBroadcastType() == BROADCASTTYPE_BROADCAST &&
                     buff.header->IsVisitedIP(NodeInfo::GetInstance()->GetHostIP())) {
                // When msg is broadcast message, and current node already received the msg before.
                // So, drop the received msg.
                delete buff.header;
                delete msg;
                break;
            }
            else if (buff.header->GetBroadcastType() == BROADCASTTYPE_BROADCAST) {
                // When msg is broadcast message, and current node does not received the msg before.
                // So, relay the received broadcast msg.
                buff.header->AppendVisitedIP(NodeInfo::GetInstance()->GetHostIP());
                RelayBroadcastMsg(buff.header, msg);
            }

            // When msg is unicast message and current node is the destination
            // Or, when msg is broadcast message, and the msg is received for the first time, i.e., not dropped.
            // So, forward the received msg to proper module if it is not proxy.
            // Also, if the current node is network participant node, do not forward.
            if (!amIProxyNode && !amINetworkParticipantNode) {
                switch (msg->GetType()) {
                case Message::TEST_MESSAGE:
                    {
                        // std::cout << "Deserialization of the received message complete!" << "\n";
                        // std::cout << "MESSAGE TYPE = TEST MESSAGE" << "\n";
                        delete buff.header;
                        delete msg;
                        break;
                    }
                case Message::TX_MESSAGE:
                    {
                        // std::cout << "Deserialization of the received message complete!" << "\n";
                        // std::cout << "MESSAGE TYPE = TX MESSAGE" << "\n";
                        Transaction *tx = dynamic_cast<Transaction*>(msg);
                        boost::shared_ptr<Transaction> shared_tx(tx);
                        handleTransactionClass->HandleArrivedTx(shared_tx);
                        delete buff.header;
                        // do not delete msg since it will be automatically managed as smart pointer
                        break;
                    }
                case Message::CONSENSUS_MESSAGE:
                    {
                        // std::cout << "Deserialization of the received message complete!" << "\n";
                        // std::cout << "MESSAGE TYPE = CONSENSUS MESSAGE" << "\n";
                        ConsensusMessage *conmsg = dynamic_cast<ConsensusMessage*>(msg);
                        handleConsensusClass->HandleArrivedConsensusMsg(conmsg);
                        delete buff.header;
                        delete msg;
                        break;
                    }
                }

                // fill here with parsing logic
                // ...
                // ...
                // CentralizedNetworkMessage nmsg = GetDeserializedMsg(p->recv_str);
            }
            break;
        }
    }

    return;
}

void HandleNetwork_ProxyModel::onSendSocketReadyEvent(std::shared_ptr<EventInfo> info) {
    std::shared_ptr<SendSocketReadyEventInfo> derivedInfo = std::static_pointer_cast<SendSocketReadyEventInfo>(info);

    int fd = derivedInfo->GetSocketFD();

    auto it = peerMap.find(fd);
    if (it == peerMap.end()) {
        std::cout << "No valid peer exists" << "\n";
        exit(-1);
    }

    Peer* peer = it->second;

    SocketSendBuffer &buff = sendBuffMap[peer->GetSocketFD()];

    if (buff.sendMsgQueue.empty()) {
        socketEventPublisher.UnsetSocketWrite(fd);
        return;
    }

    std::shared_ptr<WriteMsg> msg = buff.sendMsgQueue.front();
    int numbytes = send(fd, msg->dpos(), msg->nbytes(), 0);
    if (numbytes < 0) {
        perror("write error");
        exit(-1);
    }

    msg->pos += numbytes;
    if (msg->nbytes() == 0) {
        buff.sendMsgQueue.pop_front();
        // delete msg;
    }
}

void HandleNetwork_ProxyModel::PrintGossipPeerList() {
    std::cout << "[NetworkGraph] "<< utility::GetCurrentTime() << " ";
    std::list<Peer*>& glist = gossipPeerList.GetPeerList();
    size_t size = glist.size();
    if (size == 0) {
        std::cout << '\n';
        return;
    }
    int cnt = 1;
    for (auto& entry : glist) {
        std::cout << entry->GetIP() <<"(1)";
        if (cnt == size) std::cout << '\n';
        else std::cout << ',';
        cnt = cnt+1;
    }
}
