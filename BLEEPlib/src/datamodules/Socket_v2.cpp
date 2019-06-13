#include "Socket_v2.h"
#include "../datamanagermodules/SocketManager_v2.h"
#include "../utility/Assert.h"

#include <fcntl.h> /* Added for the nonblocking socket */
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/serialization/export.hpp>

#include <iostream>

#include "Message.h"


using namespace libBLEEP;

libBLEEP::ListenSocket_v2::ListenSocket_v2(int port) {

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


int libBLEEP::ListenSocket_v2::DoAccept() {
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

libBLEEP::ListenSocket_v2::~ListenSocket_v2() {
    if ( close(_fd) == -1) {
        perror("close");
        M_Assert(0, "error on close listen socket");
    }
}


libBLEEP::DataSocket_v2::DataSocket_v2(int sfd) {
    _fd = sfd;
}

std::pair< bool, std::shared_ptr<Message> > libBLEEP::DataSocket_v2::DoRecv() {
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
            n = recv(_fd, &length, sizeof(int),0);
            if (n == -1 && errno != EAGAIN){
                perror("recv - non blocking \n");
                std::cout << "errno=" << errno << "\n";
                exit(-1);
            }
            else if (n == 0) {
                std::cout << "socket disconnected" << "\n";

                return std::make_pair(false, nullptr);
                // notify closeEvent
                // _manager->RemoveDataSocket(_fd); // remove myself(DataSocket) from manager
            }
            else if (n > 0) {
                _recvBuff.message_len = length;
                _recvBuff.recv_status = RECV_MSG;
                _recvBuff.received_len = 0;
                _recvBuff.recv_str = "";
            }
            // break; // directly handler RECV_MSG after RECV_IDLE
        }
    case RECV_MSG:
        {
            int total_recv_size = _recvBuff.received_len;
            int numbytes = 0;
            // Handle all pending 'recv'  
            while(1) {
                int recv_size = std::min(2000, _recvBuff.message_len - total_recv_size);
                numbytes = recv(_fd, string_read, recv_size, 0);
                if (numbytes > 0) {
                    total_recv_size += numbytes;
                    _recvBuff.recv_str.append(string_read, numbytes);
                }
                else if (numbytes == 0) {
                    std::cout << "connection closed while recv\n";

                    return std::make_pair(false, nullptr);
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
                memset(string_read, 0, 2000);
            }
            if (_recvBuff.message_len != total_recv_size) {
                _recvBuff.received_len = total_recv_size;
                // std::cout << "received only part of message (maybe recv buffer is full)" << "received_len:" << _recvBuff.received_len << ", message_len:" << _recvBuff.message_len << "\n";
                break;
            }
            else {
                // std::cout << "fully received. size:" << total_recv_size << "\n";
                _recvBuff.recv_status = RECV_IDLE;

                // deserialization process //
                Message *msg;
                boost::iostreams::basic_array_source<char> device(_recvBuff.recv_str.c_str(), _recvBuff.recv_str.size());
                boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
                boost::archive::binary_iarchive ia(s);
                ia >> msg;

                return std::make_pair(true, std::shared_ptr<Message>(msg));
            }
        }
    }
    return std::make_pair( true, nullptr);
}

DoSendResultEnum libBLEEP::DataSocket_v2::DoSend() {
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

void libBLEEP::DataSocket_v2::AppendMessageToSendBuff(std::shared_ptr<Message> message) {
    Message* msg = message.get();

    std::string serial_str;
    // serialize message obj into an std::string message
    boost::iostreams::back_insert_device<std::string> inserter(serial_str);
    boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
    boost::archive::binary_oarchive oa(s);
    oa << msg;
    s.flush();

    int message_len = serial_str.size();

    _sendBuff.push_back(std::make_shared<WriteMsg>((char*)&message_len, sizeof(int)));
    _sendBuff.push_back(std::make_shared<WriteMsg>(serial_str.c_str(), message_len));
    // _manager->SetWritable(_fd);
}

void libBLEEP::DataSocket_v2::AppendMessageToSendBuff(std::shared_ptr<Message> message, PeerId dest) {
    Message* msg = message.get();

    std::string serial_str;
    // serialize message obj into an std::string message
    boost::iostreams::back_insert_device<std::string> inserter(serial_str);
    boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
    boost::archive::binary_oarchive oa(s);
    oa << msg;
    s.flush();

    int message_len = serial_str.size();

    _sendBuff.push_back(std::make_shared<WriteMsg>((char*)&message_len, sizeof(int)));
    _sendBuff.push_back(std::make_shared<WriteMsg>(serial_str.c_str(), message_len, dest, message));
    // _manager->SetWritable(_fd);
}

libBLEEP::DataSocket_v2::~DataSocket_v2() {
    if ( close(_fd) == -1) {
        perror("close");
        M_Assert(0, "error on close data socket");
    }
}

libBLEEP::ConnectSocket_v2::ConnectSocket_v2(std::string domain) {
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


