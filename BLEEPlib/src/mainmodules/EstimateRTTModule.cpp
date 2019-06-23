#include "EstimateRTTModule.h"
#include "../utility/Random.h"

#include <fcntl.h> /* Added for the nonblocking socket */
#include <arpa/inet.h>
#include <netdb.h>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>


using namespace libBLEEP;


libBLEEP::RTTModule_ListenSocket::RTTModule_ListenSocket(int port) {

    int 			listenfd;     /* listen on sock_fd */
    struct 	sockaddr_in 	my_addr;    /* my address information */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    my_addr.sin_family = AF_INET;         /* host byte order */
    my_addr.sin_port = htons(port);     /* short, network byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
    bzero(&(my_addr.sin_zero), 8);        /* zero the rest of the struct */

    if (bind(listenfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(1);
    }
    if (listen(listenfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    int flags = fcntl(listenfd, F_GETFL, 0);
    fcntl(listenfd, F_SETFL, flags | O_NONBLOCK); /* Change the socket into non-blocking state	*/
    
    _fd = listenfd;

}


int libBLEEP::RTTModule_ListenSocket::DoAccept() {
    struct 	sockaddr_in 	their_addr; /* connector's address information */
    int sock_fd;
    socklen_t 			sin_size;

    sin_size = sizeof(struct sockaddr_in);
    sock_fd = accept(_fd, (struct sockaddr *)&their_addr, &sin_size);
    if ( sock_fd != -1 ) {
        std::cout << "server: got connection from " << inet_ntoa(their_addr.sin_addr) << "\n";

        fcntl(sock_fd, F_SETFL, O_NONBLOCK);

        return sock_fd;
    }
    else {
        if( errno != EAGAIN && errno != EWOULDBLOCK ) {
            std::cout << "accept() failed errno=" << errno << strerror(errno) << "\n";
            exit(-1);
        }
        return -1;
    }
}

libBLEEP::RTTModule_ListenSocket::~RTTModule_ListenSocket() {
    if ( close(_fd) == -1) {
        perror("close");
        M_Assert(0, "error on close listen socket");
    }
}


libBLEEP::RTTModule_DataSocket::RTTModule_DataSocket(int sfd) {
    _fd = sfd;
}

std::pair< bool, std::pair< std::shared_ptr<RTTModule_MessageHeader>, std::shared_ptr<Message> > > libBLEEP::RTTModule_DataSocket::DoRecv() {
    char string_read[2000];
    int n;
    switch (_recvBuff.recv_status) {
    case RECV_NONE:
        {
            std::cout << "invalid recv state" << "\n";
            exit(-1);
        }
    case RECV_IDLE:
        {
            int length = 0;
            PrintTimespec("before recv length");
            n = recv(_fd, &length, sizeof(int),0);
            PrintTimespec("after recv length");
            if (n == -1 && errno != EAGAIN){
                perror("recv - non blocking \n");
                std::cout << "errno=" << errno << "\n";
                exit(-1);
            }
            else if (n == 0) {
                std::cout << "socket disconnected" << "\n";

                return std::make_pair(false, std::make_pair(nullptr, nullptr) );
                // notify closeEvent
                // _manager->RemoveDataSocket(_fd); // remove myself(DataSocket) from manager
            }
            else if (n > 0) {
                _recvBuff.message_len = length;
                _recvBuff.received_len = 0;
                _recvBuff.recv_str = "";

                int flags = fcntl(_fd, F_GETFL, 0);
                fcntl(_fd, F_SETFL, flags & (~O_NONBLOCK));

                
                RTTModule_MessageHeader header;
                char src[20];
                if (recv(_fd, &header.type, sizeof(int), 0) <=0) {
                    std::cout << "error while receiving header" << "\n";
                    exit(-1);
                } else {
                    std::cout << "recv type:" << header.type << "\n";
                }
                if (recv(_fd, &header.hop, sizeof(int), 0) <=0) {
                    std::cout << "error while receiving header" << "\n";
                    exit(-1);
                } else {
                    std::cout << "recv hop:" << header.hop << "\n";
                }
                if (recv(_fd, src, 20, 0) <=0) {
                    std::cout << "error while receiving header" << "\n";
                    exit(-1);
                } else {
                    std::cout << "recv src:" << src << "\n";
                }
                header.src = PeerId(std::string(src));
                if (recv(_fd, &header.timestamp, sizeof(double), 0) <=0) {
                    std::cout << "error while receiving header" << "\n";
                    exit(-1);
                } else {
                    std::cout << "recv timestamp:" << header.timestamp << "\n";
                }
                _recvBuff.recv_status = RECV_MSG;
                flags = fcntl(_fd, F_GETFL, 0);
                fcntl(_fd, F_SETFL, flags | O_NONBLOCK);                    

                PrintTimespec("receive messageheader");
                if (header.type == MESSAGETYPE_RTTREQ || header.type == MESSAGETYPE_RTTREP) {
                    // make a recv blocking
                    flags = fcntl(_fd, F_GETFL, 0);
                    fcntl(_fd, F_SETFL, flags & (~O_NONBLOCK));
                    // recv all message in single step.  
                }
                else {
                    return std::make_pair(true, std::make_pair( std::make_shared<RTTModule_MessageHeader>(header), 
                                                                nullptr));
                }
            }
        }
    case RECV_MSG:
        {
            int total_recv_size = _recvBuff.received_len;
            int numbytes = 0;
            // Handle all pending 'recv'  
            while(1) {
                int recv_size = std::min(2000, _recvBuff.message_len - total_recv_size);
                PrintTimespec("before recv msg");
                numbytes = recv(_fd, string_read, recv_size, 0);
                PrintTimespec("after recv msg");
                if (numbytes > 0) {
                    total_recv_size += numbytes;
                    _recvBuff.recv_str.append(string_read, numbytes);
                }
                else if (numbytes == 0) {
                    std::cout << "connection closed while recv\n";

                    return std::make_pair(false, std::make_pair( nullptr, nullptr) );
                    // notify  closeEvent
                    // _manager->RemoveDataSocket(_fd); // remove myself(DataSocket) from manager
                    // break;
                }
                else if (numbytes < 0) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        std::cout << "recv failed errno=" << errno << strerror(errno) << "\n";
                        exit(-1);
                    }
                    break;
                }

                if (total_recv_size == _recvBuff.message_len)
                    break;
                else {
                    // std::cout << "recv: total_recv_size=" << total_recv_size << ", message_len=" << _recvBuff.message_len << "\n";
                }
                PrintTimespec("before memset 0 recvbuff");
                memset(string_read, 0, 2000);
                PrintTimespec("after memset 0 recvbuff");
            }
            if (_recvBuff.message_len != total_recv_size) {
                PrintTimespec("received only part of message");
                _recvBuff.received_len = total_recv_size;
                // std::cout << "received only part of message (maybe recv buffer is full)" << "received_len:" << _recvBuff.received_len << ", message_len:" << _recvBuff.message_len << "\n";
                break;
            }
            else {
                PrintTimespec("fully receive message");
                // std::cout << "fully received. size:" << total_recv_size << "\n";
                _recvBuff.recv_status = RECV_IDLE;

                PrintTimespec("before deserialization message");
                // deserialization process //
                Message *msg;
                boost::iostreams::basic_array_source<char> device(_recvBuff.recv_str.c_str(), _recvBuff.recv_str.size());
                boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
                boost::archive::binary_iarchive ia(s);
                ia >> msg;

                PrintTimespec("after deserialization message");

                return std::make_pair(true, std::make_pair( nullptr ,std::shared_ptr<Message>(msg)));
            }
        }
    }
    return std::make_pair( true, std::make_pair( nullptr, nullptr) );
}

DoSendResultEnum libBLEEP::RTTModule_DataSocket::DoSend() {
    if (_sendBuff.empty()) {
        // _manager->UnsetWritable(_fd);
        return DoSendResultEnum::SendBuffEmptied; 
    }
    
    while (!_sendBuff.empty()) {
        std::shared_ptr<WriteMsg> msg = _sendBuff.front();
        int numbytes = send(_fd, msg->dpos(), msg->nbytes(), 0);
        if (numbytes < 0) {
            perror("write error");
            exit(-1);
        }

        // std::cout << "DoSend: write " << numbytes << " bytes" << "\n";

        msg->pos += numbytes;
        if (msg->nbytes() == 0) {
            if (msg->msgInfo.second != nullptr) {
                // add timestamp
                struct timespec tspec;
                clock_gettime(CLOCK_MONOTONIC, &tspec);
                char name[100];
                sprintf(name, "MsgSent(%s)", msg->msgInfo.first.GetId().c_str());
                blocktimelogs[msg->msgInfo.second->GetMessageId()][name] = tspec;
            }
            _sendBuff.pop_front();
            if (_sendBuff.empty()) {
                // _manager->UnsetWritable(_fd);
                return DoSendResultEnum::SendBuffEmptied; 
            }
        } else {
            return DoSendResultEnum::none;
        }
    }
    return DoSendResultEnum::none;
}

void libBLEEP::RTTModule_DataSocket::AppendMessageToSendBuff(RTTModule_MessageHeader header, std::shared_ptr<Message> message) {
    Message* msg = message.get();

    std::string serial_str;
    // serialize message obj into an std::string message
    boost::iostreams::back_insert_device<std::string> inserter(serial_str);
    boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
    boost::archive::binary_oarchive oa(s);
    oa << msg;
    s.flush();

    int message_len = serial_str.size();
    int type = header.type;
    int hop = header.hop;
    char src[20];
    sprintf(src, "%s", header.src.GetId().c_str());
    double timestamp = header.timestamp;

    _sendBuff.push_back(std::make_shared<WriteMsg>((char*)&message_len, sizeof(int)));
    _sendBuff.push_back(std::make_shared<WriteMsg>((char*)&type, sizeof(int)));
    _sendBuff.push_back(std::make_shared<WriteMsg>((char*)&hop, sizeof(int)));
    _sendBuff.push_back(std::make_shared<WriteMsg>(src, 20));
    _sendBuff.push_back(std::make_shared<WriteMsg>((char*)&timestamp, sizeof(double)));
    _sendBuff.push_back(std::make_shared<WriteMsg>(serial_str.c_str(), message_len));
    // _manager->SetWritable(_fd);
}

// void libBLEEP::RTTModule_DataSocket::AppendMessageToSendBuff(std::shared_ptr<Message> message, PeerId dest) {
//     Message* msg = message.get();

//     std::string serial_str;
//     // serialize message obj into an std::string message
//     boost::iostreams::back_insert_device<std::string> inserter(serial_str);
//     boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
//     boost::archive::binary_oarchive oa(s);
//     oa << msg;
//     s.flush();

//     int message_len = serial_str.size();

//     _sendBuff.push_back(std::make_shared<WriteMsg>((char*)&message_len, sizeof(int)));
//     _sendBuff.push_back(std::make_shared<WriteMsg>(serial_str.c_str(), message_len, dest, message));
//     // _manager->SetWritable(_fd);
// }

libBLEEP::RTTModule_DataSocket::~RTTModule_DataSocket() {
    if ( close(_fd) == -1) {
        perror("close");
        M_Assert(0, "error on close data socket");
    }
}

libBLEEP::RTTModule_ConnectSocket::RTTModule_ConnectSocket(std::string domain) {
    int remote_fd;
    if ( (remote_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        M_Assert(0, "error on open connect socket");
    }

    int flags = fcntl(remote_fd, F_GETFL, 0);
    fcntl(remote_fd, F_SETFL, flags | O_NONBLOCK); /* Change the socket into non-blocking state	*/

    struct addrinfo* servinfo;
    int n = getaddrinfo(domain.c_str(), NULL, NULL, &servinfo);
    if (n != 0) {
        std::cout << "Value of errno: " << errno << "\n";
        fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(n));
        perror("getaddrinfo");
        std::cout << "domain" << domain << "\n";
        M_Assert(0, "error : getaddrinfo ");
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(DEFAULT_SOCKET_PORT);
    servaddr.sin_addr.s_addr = ((struct sockaddr_in*) (servinfo->ai_addr))->sin_addr.s_addr;

    n = connect(remote_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if (n < 0 && errno != EINPROGRESS) {
        perror("connect");
        std::cout << "Unable to connect to " << domain << "\n";
        M_Assert(0, "connect error returned");
    }
    else if (n == 0) {
        std::cout << "connection established" << "\n";
        M_Assert(0, "non-blocking socket is immediately suceeded. is it possible?");
    }
    else if (n > 0) {
        M_Assert(0, "unexpected return value of non-blocking connect");
    }

    _fd = remote_fd;
}

int libBLEEP::RTTModule_SocketManager::CreateListenSocket(int port) {
    // new ListenSocket
    std::shared_ptr<RTTModule_ListenSocket> new_socket = std::make_shared<RTTModule_ListenSocket>(port);
    _listenSockets[new_socket->GetFD()] = new_socket;

    return new_socket->GetFD();
}

std::shared_ptr<RTTModule_ListenSocket> libBLEEP::RTTModule_SocketManager::GetListenSocket(int fd) {
    auto it = _listenSockets.find(fd);
    if (it == _listenSockets.end()) {
        std::cout << "No valid listenSocket exists" << "\n";
        return nullptr;
    }
    return it->second;
}




void libBLEEP::RTTModule_SocketManager::CreateDataSocket(int sfd) {
    // new DataSocket
    std::shared_ptr<RTTModule_DataSocket> new_socket = std::make_shared<RTTModule_DataSocket>(sfd);
    _dataSockets[new_socket->GetFD()] = new_socket;
    
    std::cout << "DataSocket created" << "\n";
}

void libBLEEP::RTTModule_SocketManager::RemoveDataSocket(int fd) {
    auto it = _dataSockets.find(fd);
    if ( it != _dataSockets.end())
        _dataSockets.erase(it);

}

std::shared_ptr<RTTModule_DataSocket> libBLEEP::RTTModule_SocketManager::GetDataSocket(int fd) {
    auto it = _dataSockets.find(fd);
    if (it == _dataSockets.end()) {
        std::cout << "No valid dataSocket exists" << "\n";
        return nullptr;
    }
    return it->second;
}

std::vector< std::shared_ptr<RTTModule_DataSocket> > libBLEEP::RTTModule_SocketManager::GetAllDataSockets() {
    std::vector< std::shared_ptr<RTTModule_DataSocket> > sockets;
    transform(_dataSockets.begin(), _dataSockets.end(), back_inserter(sockets), [](const std::map<int, std::shared_ptr<RTTModule_DataSocket> >::value_type& val){return val.second;} );

    return sockets;
}

int libBLEEP::RTTModule_SocketManager::CreateNonblockConnectSocket(std::string dest) {
    // new ConnectSocket
    std::shared_ptr<RTTModule_ConnectSocket> new_socket = std::make_shared<RTTModule_ConnectSocket>(dest);
    _connectSockets[new_socket->GetFD()] = new_socket;

    int fd = new_socket->GetFD();
    // M_Assert(_connectSocketWatchers.find(fd) == _connectSocketWatchers.end(), "fd must not be duplicated");

    // check https://stackoverflow.com/a/27553958 for emplacement of map structure
    // ev::io& socketIOW = _connectSocketWatchers[fd]; // newly constructed io watcher is returned
    // socketIOW.set<ConnectSocketManager, &ConnectSocketManager::_connectSocketIOCallback> (this);
    // socketIOW.start(fd, ev::WRITE);

    return fd;
}

void libBLEEP::RTTModule_SocketManager::RemoveConnectSocket(int fd) {
    auto it = _connectSockets.find(fd);
    if ( it != _connectSockets.end())
        _connectSockets.erase(it);

    // auto it_w = _connectSocketWatchers.find(fd);
    // if ( it_w != _connectSocketWatchers.end())
    //     _connectSocketWatchers.erase(it_w);
}



EstimateRTTModule::EstimateRTTModule(std::string myPeerId, double sendStarttime, int sendNum, MainEventManager* mainEventManager, int fanOutNum)
    : watcherManager(this, mainEventManager) {
    fanOut = fanOutNum;
    _mainEventManager = mainEventManager;

    // append shadow api log
    char buf2[256];
    sprintf(buf2, "API,EstimateRTTModuleConstructor,%s", myPeerId.c_str());
    shadow_push_eventlog(buf2);

    int listenSocketFD = socketManager.CreateListenSocket();
    peerManager.InitMyPeerId(myPeerId);
    new ListenSocketWatcher(listenSocketFD, this, _mainEventManager);

    if (sendNum != 0) {
        new AsyncGenerateRTTMsgTimer(sendStarttime, sendNum, this, _mainEventManager);
    }


    // append shadow log
    char buf[256];
    sprintf(buf, "InitPeerId,%s", myPeerId.c_str());
    shadow_push_eventlog(buf);
}

bool EstimateRTTModule::InsertMessageSet(std::string messageId) {
    return messageSet.insert(messageId).second;
}

bool EstimateRTTModule::ExistMessage(std::string messageId) {
    auto itr = messageSet.find(messageId);
    return (itr != messageSet.end());
}

bool EstimateRTTModule::AsyncConnectPeer(PeerId id, double time) {
    // check whether the valid socket connection already exists for given peerId
    if (peerManager.HasEstablishedDataSocket(id))
        return false;

    // append shadow api log
    char buf[256];
    sprintf(buf, "API,AsyncConnectPeer,%s,%f", id.GetId().c_str(), time);
    shadow_push_eventlog(buf);

    if (time > 0) {
        new AsyncConnectTimer(id, time, this, _mainEventManager); //timer automatically started
        return true;
    } else {
        int connecting_fd = socketManager.CreateNonblockConnectSocket(id.GetId());
        _asyncConnectPeerRequests.push_back(std::make_pair(id, connecting_fd));

        // create event watcher (ConnectSocketWatcher) for the newly created ConnectSocket
        new ConnectSocketWatcher(connecting_fd, this, _mainEventManager);
        return true;
    }
}

std::set<RTTModule_Distance, RTTModule_DistanceCmp> RTTModule_genNeighborPeerSet(PeerId myId, std::vector<PeerId> &neighborPeerIds){
    std::set<RTTModule_Distance, RTTModule_DistanceCmp> neighborPeerIdSet;
    UINT256_t myHashId = myId.GetIdHash();
    for(auto peer : neighborPeerIds){
        UINT256_t peerHashId = peer.GetIdHash();
        UINT256_t distance = myHashId ^ peerHashId;
        neighborPeerIdSet.insert(RTTModule_Distance(distance, peer));
    }
    return neighborPeerIdSet;
}

bool EstimateRTTModule::AsyncConnectPeers(std::vector<PeerId> &peerList, int peerNum, double time){
    char buf[256];
    sprintf(buf, "API,AsyncConnectPeers,%d,%d,%f",
            (int)peerList.size(),
            peerNum,
            time);
    shadow_push_eventlog(buf);

    PeerId myId = *peerManager.GetMyPeerId();
    auto neighborPeerIdSet = RTTModule_genNeighborPeerSet(myId, peerList);
    int i = 0;
    for(const RTTModule_Distance& dest : neighborPeerIdSet){
        if (i >= peerNum) break;
        if (EstimateRTTModule::AsyncConnectPeer(dest.GetPeerId(), time) == true)
            i++;
    }
    return true;
}


// sending 1-hop message
bool EstimateRTTModule::UnicastMessage(PeerId dest, std::shared_ptr<Message> message) {
    // check whether there exists a data socket for the destination peer
    if (!peerManager.HasEstablishedDataSocket(dest))
        return false;

    // append shadow api log
    char buf2[256];
    sprintf(buf2, "API,UnicastMessage,%s,%s",
            dest.GetId().c_str(),
            message->GetType().c_str());
    shadow_push_eventlog(buf2);

    // get datasocket
    int socketFD = peerManager.GetConnectedSocketFD(dest);
    std::shared_ptr<RTTModule_DataSocket> dataSocket = socketManager.GetDataSocket(socketFD);

    M_Assert(dataSocket != nullptr, "dataSocket must exist for neighbor");


    // make header for message
    struct timespec tspec;
    clock_gettime(CLOCK_MONOTONIC, &tspec);
    RTTModule_MessageHeader header;
    header.type = 0;
    if (message->GetType() == "RTTReq")
        header.type = MESSAGETYPE_RTTREQ;
    header.hop = 0;
    header.src = *peerManager.GetMyPeerId();
    header.timestamp = tspec.tv_nsec + tspec.tv_sec * 1e9;


    // append a message to socket
    int flags = fcntl(socketFD, F_GETFL, 0);
    fcntl(socketFD, F_SETFL, flags & (~O_NONBLOCK));

    dataSocket->AppendMessageToSendBuff(header, message); // sending 1-hop message
    DoSendResultEnum result = dataSocket->DoSend();
    M_Assert(result == DoSendResultEnum::SendBuffEmptied, "must be sent");
    flags = fcntl(socketFD, F_GETFL, 0);
    fcntl(socketFD, F_SETFL, flags | O_NONBLOCK);

    if (message->GetType() == "RTTReq") {
        char msg[100];
        sprintf(msg, "RTTReq sent to %s", dest.GetId().c_str());
        PrintTimespec(msg);
    }

    // // set writable for data socket watcher
    // std::shared_ptr<DataSocketWatcher> dataSocketWatcher = watcherManager.GetDataSocketWatcher(socketFD);
    // if (dataSocketWatcher)
    //     dataSocketWatcher->SetWritable();
    // else
    //     M_Assert(0, "proper dataSocketWatcher not exist");

    // append shadow log
    char buf[256];
    sprintf(buf, "UnicastMessage,%s,%s,%s,%s",
            peerManager.GetMyPeerId()->GetId().c_str(),
            dest.GetId().c_str(),
            message->GetType().c_str(),
            message->GetMessageId().c_str());
    shadow_push_eventlog(buf);

    return true;
}

bool EstimateRTTModule::MulticastMessage(std::shared_ptr<Message> message) {
    char buf[256];
    sprintf(buf, "API,MulticastMessage,%s", message->GetType().c_str());
    shadow_push_eventlog(buf);

    PeerId myId = *peerManager.GetMyPeerId();
    std::vector<PeerId> dests = peerManager.GetNeighborPeerIds();
    if (dests.size() == 0) return true;
    auto idxs = GenRandomNumSet(dests.size(), fanOut+1);
    
    // for(std::vector<PeerId>::size_type i = 0 ; i < dests.size(); i++){
    //     if (idxs.find(i) != idxs.end()){
    int sentOut = 0;
    for (int i : idxs) {
        if (message->GetSource().GetId() != dests[i].GetId() && sentOut < fanOut) {
            // get datasocket
            int socketFD = peerManager.GetConnectedSocketFD(dests[i]);
            std::shared_ptr<RTTModule_DataSocket> dataSocket = socketManager.GetDataSocket(socketFD);

            M_Assert(dataSocket != nullptr, "dataSocket must exist for neighbor");

            // append shadow log
            char buf[256];
            sprintf(buf, "MulticastingMessage,%s,%s,%s,%s",
                    peerManager.GetMyPeerId()->GetId().c_str(),
                    dests[i].GetId().c_str(),
                    message->GetType().c_str(),
                    message->GetMessageId().c_str());
            shadow_push_eventlog(buf);


            // make header for message
            struct timespec tspec;
            clock_gettime(CLOCK_MONOTONIC, &tspec);
            RTTModule_MessageHeader header;
            header.type = 0;
            if (message->GetType() == "RTTReq")
                header.type = MESSAGETYPE_RTTREQ;
            header.hop = 0;
            header.src = *peerManager.GetMyPeerId();
            header.timestamp = tspec.tv_nsec + tspec.tv_sec * 1e9;


            // append a message to socket
            int flags = fcntl(socketFD, F_GETFL, 0);
            fcntl(socketFD, F_SETFL, flags & (~O_NONBLOCK));

            dataSocket->AppendMessageToSendBuff(header, message); // sending 1-hop message
            DoSendResultEnum result = dataSocket->DoSend();
            M_Assert(result == DoSendResultEnum::SendBuffEmptied, "must be sent");
            flags = fcntl(socketFD, F_GETFL, 0);
            fcntl(socketFD, F_SETFL, flags | O_NONBLOCK);
            sentOut++;
        }
    }
    return true;

}

bool EstimateRTTModule::ForwardMessage(std::shared_ptr<RTTModule_MessageHeader> recvheader, std::shared_ptr<Message> message) {
    char buf[256];
    sprintf(buf, "API,ForwardMessage,%s", message->GetType().c_str());
    shadow_push_eventlog(buf);

    PeerId myId = *peerManager.GetMyPeerId();
    std::vector<PeerId> dests = peerManager.GetNeighborPeerIds();
    if (dests.size() == 0) return true;
    auto idxs = GenRandomNumSet(dests.size(), fanOut+1);
    
    int sentOut = 0;
    for (int i : idxs) {
        if (message->GetSource().GetId() != dests[i].GetId() && sentOut < fanOut){
            // get datasocket
            int socketFD = peerManager.GetConnectedSocketFD(dests[i]);
            std::shared_ptr<RTTModule_DataSocket> dataSocket = socketManager.GetDataSocket(socketFD);

            M_Assert(dataSocket != nullptr, "dataSocket must exist for neighbor");

            // append shadow log
            char buf[256];
            sprintf(buf, "MulticastingMessage,%s,%s,%s,%s",
                    peerManager.GetMyPeerId()->GetId().c_str(),
                    dests[i].GetId().c_str(),
                    message->GetType().c_str(),
                    message->GetMessageId().c_str());
            shadow_push_eventlog(buf);

            // make header for message
            struct timespec tspec;
            clock_gettime(CLOCK_MONOTONIC, &tspec);
            RTTModule_MessageHeader header;
            header.type = recvheader->type;
            header.hop = recvheader->hop + 1;
            header.src = recvheader->src;
            header.timestamp = recvheader->timestamp;


            // append a message to socket
            int flags = fcntl(socketFD, F_GETFL, 0);
            fcntl(socketFD, F_SETFL, flags & (~O_NONBLOCK));

            dataSocket->AppendMessageToSendBuff(header, message); // sending a message
            DoSendResultEnum result = dataSocket->DoSend();
            M_Assert(result == DoSendResultEnum::SendBuffEmptied, "must be sent");
            flags = fcntl(socketFD, F_GETFL, 0);
            fcntl(socketFD, F_SETFL, flags | O_NONBLOCK);
                
            sentOut++;
        }
    }
    return true;
}

bool EstimateRTTModule::DisconnectPeer(PeerId id) {
    // this API disconnects all existing socket connection for given PeerId.
    // Thus, it will not only disconnect the connection requested by myself,
    // but also disconnect the connection requested by remote.

    // check whether the socket connection exists for given peerId
    std::shared_ptr<PeerInfo> peerInfo = peerManager.GetPeerInfo(id);
    if (peerInfo == nullptr ||
        (peerInfo->GetSocketStatus() != SocketStatus::SocketConnected &&
         peerInfo->GetSocketStatusRemote() != SocketStatus::SocketConnected)) {
        // there's no valid socket connection for given peerId
        return false;
    }

    // append shadow api log
    char buf2[256];
    sprintf(buf2, "API,DisconnectPeer,%s", id.GetId().c_str());
    shadow_push_eventlog(buf2);

    if (peerInfo->GetSocketStatus() == SocketStatus::SocketConnected) {
        int socketFD = peerInfo->GetSocketFD();
        // remove dataSocket
        // it will automatically call destructor of DataSocket, thus automatically call close()
        socketManager.RemoveDataSocket(socketFD);
        // remove dataSocketWatcher
        // it will automatically call destructor of DataSocketWatcher (benefit of smart pointer)
        watcherManager.RemoveDataSocketWatcher(socketFD);

        // update peermanager for closed socket
        peerManager.UpdateNeighborSocketDisconnection(socketFD);
    }

    if (peerInfo->GetSocketStatusRemote() == SocketStatus::SocketConnected) {
        int socketFD = peerInfo->GetSocketFDRemote();
        // remove dataSocket
        // it will automatically call destructor of DataSocket, thus automatically call close()
        socketManager.RemoveDataSocket(socketFD);
        // remove dataSocketWatcher
        // it will automatically call destructor of DataSocketWatcher (benefit of smart pointer)
        watcherManager.RemoveDataSocketWatcher(socketFD);

        // update peermanager for closed socket
        peerManager.UpdateNeighborSocketDisconnection(socketFD);
    }


    // append shadow log for connection establishment
    char buf[256];
    sprintf(buf, "DisconnectPeer,%s,%s", peerManager.GetMyPeerId()->GetId().c_str(), id.GetId().c_str());
    shadow_push_eventlog(buf);

    return true;
}

void EstimateRTTModule::SendRTTMsg() {

    std::vector<PeerId> dests = peerManager.GetNeighborPeerIds();    
    std::cout << "send RTT Msg" << "\n";
    
    PeerId myPeerId = *peerManager.GetMyPeerId();

    for (PeerId peer : dests) {
        // int socketFD = peerManager.GetConnectedSocketFD(peer);
        // std::shared_ptr<DataSocket_v2> dataSocket = socketManager.GetDataSocket(socketFD);
        // if (!dataSocket) {
        //     M_Assert(0, "proper data socket not exist");
        // }
        if (peer.GetId() == myPeerId.GetId())
            continue;
    
        std::shared_ptr<Message> msg = std::make_shared<Message>(myPeerId, peer, 
                                                                 "RTTReq", "");

        UnicastMessage(peer, msg);

    }
}


