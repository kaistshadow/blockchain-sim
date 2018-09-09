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

std::string GetSerializedString(P2PMessage msg) {
  std::string serial_str;
  // serialize obj into an std::string payload
  boost::iostreams::back_insert_device<std::string> inserter(serial_str);
  boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
  boost::archive::binary_oarchive oa(s);
  oa << msg;
  s.flush();
  return serial_str;
}

P2PMessage GetDeserializedMsg(std::string str) {
  P2PMessage msg;
  // wrap buffer inside a stream and deserialize string_read into obj
  boost::iostreams::basic_array_source<char> device(str.c_str(), str.size());
  boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
  boost::archive::binary_iarchive ia(s);
  ia >> msg;
  return msg;
}

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
    perror("create epoll"); return;
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
  return sfd;
} 

void SocketInterface::SyncSendMsg(SocketMessage msg){
  SocketMessage p = msg;
  int sfd   = p.GetSocketfd();
 
  int flags = fcntl(sfd, F_GETFL, 0);
  fcntl(sfd, F_SETFL, flags & (~O_NONBLOCK));
  
  std::string payload = GetSerializedString(p.GetP2PMessage());
  int     payload_len = payload.size();
  if (payload_len <= 0) {
    std::cout << "send event: Serialization fault\n";
    return;
  }

  int numbytes = send(sfd, (char*)&payload_len, sizeof(int), 0);
  if (numbytes < sizeof(int)) {
    std::cout << "send event: network fail\n"; 
    SendFailMsg(sfd);
    return;
  }
  numbytes = send(sfd, payload.c_str(), payload_len, 0);
  if (numbytes < payload_len) {
    std::cout << "send event: network fail\n"; 
    SendFailMsg(sfd);
    return;
  }
  //std::cout << "send event: SyncSendMsg payload size: "<<numbytes<<" to "<< sfd<<'\n';

  if (p.GetMethod() == M_DISCONNECT){
    DeleteSocketDataEntry(sfd);
    close(sfd);
    return;
  }
  flags = fcntl(sfd, F_GETFL, 0);
  fcntl(sfd, F_SETFL, flags | O_NONBLOCK);
} 

void SocketInterface::SendFailMsg(int sfd){
  SocketMessage smsg = SocketMessage();
  smsg.SetMethod(M_NETWORKFAIL, sfd);
  SimpleGossipProtocol::GetInstance()->PushToQueue(smsg);

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
  for (int i=0; i<socket_view.size();i++) {
    if (socket_view[i].sfd == sfd) {
      socket_view.erase(socket_view.begin()+i);
      close(sfd);
    }
  }
}

void SocketInterface::InsertSocketData(int sfd, SocketMessage msg) {
  SocketData* entry = FindSocketDataEntry(sfd);
  if (entry) {
    entry->PushToQueue(msg);
  }
}

void SocketInterface::ProcessMsg(SocketMessage msg) {
  std::vector<int> list = msg.sockets;
  int type = msg.GetMethod();
  
  if (type == M_CONNECT) {
    if (!list.empty()) { 
      std::cout << "wrong socket list\n";
      return;
    }
    if (FindSocketDataEntryById(msg.GetDstPeer()) != NULL)
      return;

    int newsfd = ConnectToPeer(msg.GetDstPeer());
    if (newsfd != -1) {
      msg.SetSocketfd(newsfd);
      InsertSocketData(newsfd, msg);
      SetEvent(EPOLL_CTL_ADD, EPOLLOUT, newsfd);
    }
    return;
  }
  
  if (type == M_UPDATE) {
    SocketData* entry = FindSocketDataEntry(msg.GetSocketfd()); 
    if (!entry) {
      std::cout << "can't find socket data entry\n";
      return;
    }
    if (entry->info_status == INFO_INCOMPLETE) {
      entry->SetId(msg.GetDstPeer());
    }
    return;
  }
  
  for (int i=0; i<list.size(); i++) {  
    msg.SetSocketfd(list[i]);
    SyncSendMsg(msg);
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
  struct epoll_event events[100]; 
  int num_fds = epoll_wait(ed, events, 100, 0);
  if (num_fds == -1) return;  

  for (int i=0; i<num_fds; i++) {
    int      fd = events[i].data.fd;
    uint32_t ev = events[i].events;
    
    /*Have to controll network fail cases : Push FAIL msg to Protocol*/
    if (ev & EPOLLOUT) {
      SocketData* entry = FindSocketDataEntry(fd);      
      while(!entry->msgQueue.empty()){
	SyncSendMsg(entry->msgQueue.front());
	entry->msgQueue.pop();
      }
      SetEvent(EPOLL_CTL_MOD, EPOLLIN, fd);    
    }

    if (ev & EPOLLIN) {
      if (listen_fd == fd) {
	int newfd = accept(fd, NULL, NULL);
	SocketData* entry = CreateSocketDataEntry(newfd);
	entry->SetId(std::string());

	SetEvent(EPOLL_CTL_ADD, EPOLLIN, newfd);
	continue;
      }      

      char buffer[2000];
      memset(buffer, 0, 2000);
      SocketData* entry = FindSocketDataEntry(fd);
      if (!entry) continue;
     
      switch (entry->status) {
	case RECV_IDLE:
	  {
	    int len=0;
	    int numbytes = recv(fd, &len, sizeof(int), 0);
	    if (numbytes <= 0) {
	      if (numbytes == 0)
		std::cout << "connection closed\n";	      
	      else
		std::cout << "recv length fail\n";	      
	      SendFailMsg(fd);
	      return;
	    }
       	    entry->status      = RECV_LENGTH;
	    entry->payload_len = len;
	  } 
	  break;
	
	case RECV_LENGTH:
	  {
	    int numbytes = recv(fd, buffer, entry->payload_len, 0);
	    if (numbytes <= 0) {
	      std::cout << "recv packet fail\n";	      
	      SendFailMsg(fd);
	      return;
	    }
	    std::string str(buffer, entry->payload_len);
	    P2PMessage  pmsg = GetDeserializedMsg(str);

	    SocketMessage smsg = SocketMessage();
	    smsg.SetP2PMessage(pmsg);   
	    if (entry->info_status == INFO_INCOMPLETE)
	      smsg.SetMethod(M_UPDATE, fd);
	    else
	      smsg.SetSocketfd(fd);
	    SimpleGossipProtocol::GetInstance()->PushToQueue(smsg);    
	    
	    entry->status      = RECV_IDLE;
	    entry->payload_len = 0;
	    //std::cout << "recv event: payload size: "<<numbytes<< " from "<<fd<<'\n';
	  }
	  break;
      }
    }
  }
}
