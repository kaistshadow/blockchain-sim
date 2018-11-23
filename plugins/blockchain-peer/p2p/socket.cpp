#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <errno.h> 
#include <assert.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h> 
#include <sys/epoll.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <fcntl.h>

#include "p2pmessage.h"
#include "socketmessage.h"
#include "socket.h"
#include "simplepeerlist.h"
#include "gossipprotocol.h"

#include "../consensus/powconsensus.h"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/device/array.hpp>
using namespace boost::archive;

SocketInterface* SocketInterface::instance = 0;
SocketInterface* SocketInterface::GetInstance() {
  if (instance == 0) {
    instance = new SocketInterface();
  }
  return instance;
}

std::string GetSerializedString(NetworkMessage msg) {
  std::string serial_str;
  // serialize obj into an std::string payload
  boost::iostreams::back_insert_device<std::string> inserter(serial_str);
  boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
  boost::archive::binary_oarchive oa(s);
  oa << msg;
  s.flush();
  return serial_str;
}

NetworkMessage GetDeserializedMsg(std::string str) {
  NetworkMessage msg;
  // wrap buffer inside a stream and deserialize string_read into obj
  boost::iostreams::basic_array_source<char> device(str.c_str(), str.size());
  boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
  boost::archive::binary_iarchive ia(s);
  ia >> msg;
  return msg;
}

// std::string GetSerializedString(P2PMessage msg) {
//   std::string serial_str;
//   // serialize obj into an std::string payload
//   boost::iostreams::back_insert_device<std::string> inserter(serial_str);
//   boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
//   boost::archive::binary_oarchive oa(s);
//   oa << msg;
//   s.flush();
//   return serial_str;
// }

// P2PMessage GetDeserializedMsg(std::string str) {
//   P2PMessage msg;
//   // wrap buffer inside a stream and deserialize string_read into obj
//   boost::iostreams::basic_array_source<char> device(str.c_str(), str.size());
//   boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
//   boost::archive::binary_iarchive ia(s);
//   ia >> msg;
//   return msg;
// }

void SocketInterface::SetEvent(int mod, int event, int fd){
  assert(ed);

  if (fd == -1) {
    perror("set_event"); return;
  }
  struct epoll_event ev;
  ev.events  = event;
  ev.data.fd = fd;
  
  int res = epoll_ctl(ed, mod, fd, &ev);
  if (res == -1){
    perror("set_event"); return;
  }
}

void SocketInterface::InitEventDescriptor() {
  int epoll = epoll_create(1);
  if (epoll == -1) {
    perror("create epoll"); 
    return;
  }
  ed = epoll;
}

void SocketInterface::InitListeningSocket() {
  assert(ed);

  int sfd = socket(AF_INET, (SOCK_STREAM | SOCK_NONBLOCK), 0);
  if (sfd == -1) {
    perror("listen_fd"); return;
  }
  struct sockaddr_in address;
  memset(&address, 0, sizeof(address));
  address.sin_family      = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port        = htons(MYPORT);
  
  int res = bind(sfd, (struct sockaddr*)&address, sizeof(address));
  if (res == -1) {
    close(sfd);
    perror("listen_fd"); return;
  }
  res = listen(sfd, BACKLOG);
  if (res == -1) {
    close(sfd); 
    perror("listen_fd"); return;
  }

  assert(sfd);
  SetEvent(EPOLL_CTL_ADD, EPOLLIN, sfd);
  listen_fd = sfd;
}

int SocketInterface::ConnectToPeer(std::string pn){  
  struct addrinfo* peerinfo;
  int res = getaddrinfo((const char*)pn.c_str(), NULL, NULL, &peerinfo);
  if (res == -1) return -1;
  
  in_addr_t ip = ((struct sockaddr_in*)(peerinfo->ai_addr))->sin_addr.s_addr;
  freeaddrinfo(peerinfo);

  int sfd = socket(AF_INET, (SOCK_STREAM | SOCK_NONBLOCK), 0);
  if (sfd == -1) return -1;

  struct sockaddr_in address;
  memset(&address, 0, sizeof(address));
  address.sin_family      = AF_INET;
  address.sin_port        = htons(MYPORT);
  address.sin_addr.s_addr = ip;
  
  res = connect(sfd, (struct sockaddr*)&address, sizeof(address));
  if (res == -1 && errno != EINPROGRESS) {
    close(sfd); return -1;
  }

  SocketData* entry= CreateSocketDataEntry(sfd);
  entry->SetId(pn);

  PrintSocketList();
  return sfd;
} 

int SocketInterface::SendSerializedMsg(SocketMessage msg){
  int sfd = msg.GetSocketfd();
  
  //P2PMSG type
  NetworkMessage nmsg(NetworkMessage_P2PMSG, msg.GetP2PMessage());
 
  std::string payload = GetSerializedString(nmsg);
  int     payload_len = payload.size();
  if (payload_len <= 0) {
    std::cerr << "send event: Serialization fault\n";
    return 0;
  }
   
  int numbytes = send(sfd, (char*)&payload_len, sizeof(int), 0);
  if (numbytes < sizeof(int)) {
    if (numbytes == 0)
      std::cerr << "send event: connection closed(1)\n";
    else
      std::cerr << "send event: network fail(1):"<<numbytes<<','<<errno<<'\n';
    SendFailMsg(sfd);
    return -1;
  }

  numbytes = send(sfd, payload.c_str(), payload_len, 0);
  if (numbytes < payload_len) {
    if (numbytes == 0)
      std::cerr << "send event: connection closed(2)\n";
    else
      std::cerr << "send event: network fail(2):"<<numbytes<<','<<errno<<'\n'; 
    SendFailMsg(sfd);
    return -1;
  }

  if (msg.GetMethod() == M_DISCONNECT) {
    DeleteSocketDataEntry(sfd);
    return -1;
  }

  return 1;
} 

void SocketInterface::SendFailMsg(int sfd){
  SocketMessage smsg = SocketMessage();
  smsg.SetMethod(M_NETWORKFAIL, sfd);
  SimpleGossipProtocol::GetInstance()->PushToQueue(smsg);

  SocketData* entry = FindSocketDataEntry(sfd);
  // for debugging
  if (entry) 
    std::cerr << "NF: from " << entry->id << ".\n";
  else
    std::cerr << "NF: no entry exists sfd = " << sfd << '\n';

  DeleteSocketDataEntry(sfd);
}

SocketData* SocketInterface::FindSocketDataEntry(int sfd) {
  for (int i=0; i<socket_view.size();i++) {
    if (socket_view[i].sfd == sfd) return &socket_view[i];
  }
  return NULL;
}

SocketData* SocketInterface::FindSocketDataEntryById(std::string pn) {
  for (int i=0; i<socket_view.size();i++) {
    if (socket_view[i].id == pn) return &socket_view[i];
  }
  return NULL;
}

SocketData* SocketInterface::CreateSocketDataEntry(int sfd) {
  SocketData entry = SocketData(sfd);
  socket_view.push_back(entry);
  return &socket_view[socket_view.size()-1];
}

void SocketInterface::DeleteSocketDataEntry(int sfd) {
  for (int i=0; i<socket_view.size(); i++) {
    if (socket_view[i].sfd == sfd) {
      socket_view.erase(socket_view.begin()+i);
      SetEvent(EPOLL_CTL_DEL, EPOLLIN, sfd);
      close(sfd);
    }
  }
  PrintSocketList();
}

/**
 * DEPRECATED (temporarily used for testing PoW consensus)
 * temporary implementation for unicast 
void SocketInterface::UnicastP2PMsg(P2PMessage msg, const char *hostname) {
    NetworkMessage nmsg(NetworkMessage_P2PMSG, msg);

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
        std::cout << "error in connection : getaddrinfo" << "\n";
        exit(1);
    }
    servaddr.sin_addr.s_addr = ((struct sockaddr_in*) (servinfo->ai_addr))->sin_addr.s_addr;

    n = connect(cli_sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    if (n < 0) {
        perror("connect");
        exit(1);
    } 
    else if (n == 0) {
        std::cout << "connection established for unicast" << "\n";
    }

    SocketMessage sockmsg;
    sockmsg.SetSocketfd(cli_sockfd);
    sockmsg.SetP2PMessage(msg);

    // send_socketmessage
    std::string payload = GetSerializedString(nmsg);
    int     payload_length = payload.size();

    n = send(cli_sockfd, (char*)&payload_length, sizeof(int), 0);
    if (n < 0){ 
        std::cout << "send errno=" << errno << "\n";
        exit(1);
    }
    else if (n < sizeof(int)) {
        std::cout << "Warning : sented string is less than requested" << "\n";
        std::cout << "sented string length: " << n << "\n";
        exit(1);
    }
    else {
        std::cout << "sented string length: " << n << "\n";
    }            


    n = send(cli_sockfd,payload.c_str(),payload_length,0);
    if (n < 0){ 
        std::cout << "send errno=" << errno << "\n";
        exit(1);
    }
    else if (n < payload_length) {
        std::cout << "Warning : sented string is less than requested" << "\n";
        std::cout << "sented string length: " << n << "\n";
        exit(1);
    }
    else {
        std::cout << "sented string length: " << n << "\n";
    }           

}
*/

int SocketInterface::InsertSocketData(int sfd, SocketMessage msg) {
  SocketData* entry = FindSocketDataEntry(sfd);
  if (entry) {
    entry->PushToQueue(msg);
    return 0;
  }
  return -1;
}

void SocketInterface::ProcessMsg(SocketMessage msg) {
  int type = msg.GetMethod();
 
  // Method for connecting to specific node
  // Now, assuming CONNECT always brings P2P Msg to be transmitted
  if (type == M_CONNECT) {//&->==
    if (!msg.sockets.empty()) { 
      std::cerr << "Msg Process: connect: non empty socket list\n";
      return;
    }
    
    SocketData* entry = FindSocketDataEntryById(msg.GetDstPeer());
    if (entry) {
      int sfd = entry->sfd;
      if (type & M_DISCONNECT) {//now, useless due to ==
	msg.SetMethod(M_DISCONNECT, sfd);
      }
      else {
	msg.SetSocketfd(sfd);
      }
      InsertSocketData(sfd, msg);
      SetEvent(EPOLL_CTL_MOD, EPOLLOUT, sfd);
      return;
    }
  
    int newsfd = ConnectToPeer(msg.GetDstPeer());
    if (newsfd != -1) {
      msg.SetSocketfd(newsfd);
      InsertSocketData(newsfd, msg);
      SetEvent(EPOLL_CTL_ADD, EPOLLOUT, newsfd);
    }
    else {
      std::cerr << "Msg Process: connect: create connection failed\n";
    }
    return;
  }
 
  // Method for maintain SocketDataEntry, update node ID related to given socket# 
  if (type == M_UPDATE) {
    SocketData* entry = FindSocketDataEntry(msg.GetSocketfd()); 
    if (!entry) {
      std::cerr << "Msg Process: update: can't find socket data entry\n";
      return;
    }
    if (entry->info_status == INFO_INCOMPLETE) {
      entry->SetId(msg.GetDstPeer());
      PrintSocketList();
    }
    return;
  }

  // Method for disconnect to specific node, delete entry from SocketDataEntry list.
  if (type == M_DISCONNECT) {
    int sfd = msg.GetSocketfd();
    DeleteSocketDataEntry(sfd);
    return;
  }
  
  // Methods that use socket# list, "sockets", for multicasting msgs or one msg.
  if (type == M_BROADCAST || type == BROADCAST || type == M_UNICAST) {
    for (int i=0; i<msg.sockets.size(); i++) {  
      int sfd = msg.sockets[i];
      msg.SetSocketfd(sfd);

      if (InsertSocketData(sfd, msg) == -1) {
	std::cerr << "Msg Process: multicast: can't find socket data entry,"<<sfd<<"\n";
	continue;
      }
      SetEvent(EPOLL_CTL_MOD, EPOLLOUT, sfd);      
      /*
      // for debug
      P2PMessage pmsg = msg.GetP2PMessage();
      if (pmsg.g_type == GOSSIP)
	std::cout << "send mid:"<< pmsg.g_mid << "\n";
      */
    }
    return;
  }
}

void SocketInterface::InitSocketInterface() {
  InitEventDescriptor();
  InitListeningSocket(); 
}

int SocketInterface::GetListenSocket() {
  return listen_fd;
}

void SocketInterface::ProcessQueue() {
  while(!msgQueue.empty()){
    ProcessMsg(msgQueue.front());
    msgQueue.pop();
  }
}

void SocketInterface::ProcessNetworkEvent() {

  struct epoll_event events[150]; 
  int num_fds = epoll_wait(ed, events, 150, 0);
  if (num_fds == -1) {
    std::cerr << "EPOLL wait error("<<errno<<")\n";
    return;
  }   

  for (int i=0; i<num_fds; i++) {
    int      fd = events[i].data.fd;
    uint32_t ev = events[i].events;

    // Control OUT event first
    if (ev & EPOLLOUT) {
      SocketData* entry = FindSocketDataEntry(fd);
      if (!entry) {
	std::cerr << "send event: no entry exists\n";
	continue;
      }
	
      int res = 1;
      while(!entry->msgQueue.empty()){
	res = SendSerializedMsg(entry->msgQueue.front());
	if (res == -1) break;
	entry->msgQueue.pop();
      }
      if (res != -1)
	SetEvent(EPOLL_CTL_MOD, EPOLLIN, fd);
    }   

    // Control IN event later
    if (ev & EPOLLIN) {
      if (listen_fd == fd) {
	int newfd = accept(fd, NULL, NULL);
	SocketData* entry = CreateSocketDataEntry(newfd);
	SetEvent(EPOLL_CTL_ADD, EPOLLIN, newfd);

	PrintSocketList();
	continue;
      }      

      char buffer[2000];
      memset(buffer, 0, 2000);
      SocketData* entry = FindSocketDataEntry(fd);
      if (!entry) {
	std::cerr << "recv event: no entry exists\n";	      
	continue;
      }
     
      switch (entry->status) {
	case RECV_IDLE:
	  {
	    int len=0;
	    int numbytes = recv(fd, &len, sizeof(int), 0);
	    if (numbytes <= 0) {
	      if (numbytes == 0)
		std::cerr << "recv event: connection closed(idle)\n";	      
	      else
		std::cerr << "recv event: recv length fail\n";	      
	      SendFailMsg(fd);
	      break;
	    }
       	    entry->status      = RECV_LENGTH;
	    entry->payload_len = len;
	  } 
	  break;
	
	case RECV_LENGTH:
	  {
            // int numbytes = recv(fd, buffer, entry->payload_len, 0);
            // if (numbytes <= 0) {
            //   if (numbytes == 0) {
            //     std::cerr << "recv event: connection closed(2)\n";
            //   }
            //   else {
            //     std::cerr << "recv event: recv payload fail\n";
            //   }
            //   SendFailMsg(fd);
            //   break;
            // }
            // std::string str(buffer, entry->payload_len);

            int total_recv_size = 0;
            int numbytes = 0;
            std::string recv_str;

            while(1) {
              int recv_size = std::min(2000, entry->payload_len - total_recv_size);
              numbytes = recv(fd, buffer, recv_size, 0);              
              if (numbytes > 0) {
                total_recv_size += numbytes;
                recv_str.append(buffer, numbytes);
              }
              else if (numbytes == 0) {
                std::cerr << "recv event: connection closed(len)\n";
                break;
              }
              else if (numbytes < 0) {
                if (errno != EWOULDBLOCK) {
                  std::cerr << "recv event: recv payload fail\n";
                  break;
                }
              }
              
	      if (total_recv_size == entry->payload_len)
                break;
              memset(buffer, 0, 2000);
            }

            if (total_recv_size != entry->payload_len) {
              std::cerr << "recv event: recv payload fail\n";	      
              SendFailMsg(fd);
              break;
            }

            NetworkMessage nmsg = GetDeserializedMsg(recv_str);
	    // P2PMessage    pmsg = GetDeserializedMsg(str);
            SocketMessage smsg;
            if (nmsg.type == NetworkMessage_P2PMSG) {
                P2PMessage pmsg = boost::get<P2PMessage>(nmsg.data);
                SocketMessage smsg = SocketMessage();
                smsg.SetP2PMessage(pmsg);   
                if (entry->info_status == INFO_INCOMPLETE) {
                    smsg.SetMethod(M_UPDATE, fd);
                }
                else {
                    smsg.SetMethod(M_NORMAL, fd);
                }
                SimpleGossipProtocol::GetInstance()->PushToQueue(smsg);    
                entry->status      = RECV_IDLE;
                entry->payload_len = 0;
            }
            else if (nmsg.type == NetworkMessage_UNIMSG) {
                entry->status      = RECV_IDLE;
                entry->payload_len = 0;
                UnicastMessage umsg = boost::get<UnicastMessage>(nmsg.data);
                switch(umsg.type) {
                case UnicastMessage_POWCONSENSUSMESSAGE:
                    POWConsensusMessage powmsg = boost::get<POWConsensusMessage>(umsg.data);
                    POWConsensus::GetInstance()->PushToQueue(powmsg);
                    std::cout << "pushed POWConsensusMessage (unicast msg) to queue" << "\n";
                    break;
                } 	        
            }
	  }
	  break;

	default:
	  break;
      }
    }
  }
}

void SocketInterface::ShutdownInterface() {
  while(!socket_view.empty()) {
    int sfd = socket_view.front().sfd;
    socket_view.erase(socket_view.begin());
    close(sfd); 
    PrintSocketList();
  }
}

void SocketInterface::PrintSocketList() {
  return;
  for (int i=0; i<socket_view.size(); i++){
    SocketData sd = socket_view[i];
    std::cout <<'('<< sd.sfd << ','<<sd.id<<')';
  }
  std::cout <<'\n';
}


// Implemented and Commented by Yonggon Kim.
// Currently, i have implemented this function with raw socket API (blocking connect & send)
// Since it utilizes blocking connect, it might suffer from long latency. 
// Later, we have to merge this function(SendUnicastMsg) into a SocketInterface APIs.
// I already have tried to merge it, but failed because I couldn't fully understand about
// how existing SocketInterface APIs works. (e.g., how it handles non-blocking connect API & sockets)
void SocketInterface::SendUnicastMsg(UnicastMessage msg, std::string dest) {
    int sfd;

    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in servaddr;
    struct addrinfo* servinfo;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);

    int n = getaddrinfo(dest.c_str(), NULL, NULL, &servinfo);
    if (n != 0) {
        std::cout << "error in connection : getaddrinfo" << "\n";
        exit(1);
    }
    servaddr.sin_addr.s_addr = ((struct sockaddr_in*) (servinfo->ai_addr))->sin_addr.s_addr;

    n = connect(sfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if (n < 0) {
        perror("connect");
        exit(1);
    } 

    NetworkMessage netmsg(NetworkMessage_UNIMSG, msg);

    std::string payload = GetSerializedString(netmsg);
    int payload_len = payload.size();

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

    close(sfd);
}
