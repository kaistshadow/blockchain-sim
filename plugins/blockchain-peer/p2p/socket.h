#ifndef P2P_SOCKET_H
#define P2P_SOCKET_H

#include <vector>
#include <queue>
#include "simplepeerlist.h"
#include "p2pmessage.h"
#include "socketmessage.h"

#define MYPORT 3456
#define BACKLOG 10 

enum RECV_STATUS {
  RECV_IDLE   = 0,
  RECV_LENGTH = 1,
};

enum INFO_STATUS {
  INFO_INCOMPLETE = 0,
  INFO_COMPLETE   = 1,
};

class SocketData {
 public:
  SocketData(int fd) {
    sfd    = fd;
    status = RECV_IDLE;
    info_status = INFO_INCOMPLETE;
  }
  int sfd;
  std::string id;
  std::queue<SocketMessage> msgQueue;   
 
  int payload_len;
  RECV_STATUS status;
  INFO_STATUS info_status;

  void SetId(std::string ID) {id = ID; info_status = INFO_COMPLETE;}
  void PushToQueue(SocketMessage msg) {msgQueue.push(msg);}
};
typedef std::vector<SocketData> SocketList;

class SocketInterface {
 private:
  SocketInterface(){};                  // singleton pattern
  static SocketInterface* instance;     // singleton pattern

  int listen_fd;  // socket for accepting incomming connect request
  int server_fd;  // socket for seed server-hostpeer connetction
  int ed;         // EPOLL descriptor for event handling
  SocketList socket_view;

  void SetEvent(int mod, int event, int fd);
  void InitEventDescriptor(); 
  void InitListeningSocket();

  int  ConnectToPeer(std::string pn);
  int  SyncSendMsg(SocketMessage msg);
  void SendFailMsg(int sfd);

  SocketData* FindSocketDataEntry(int sfd);
  SocketData* FindSocketDataEntryById(std::string pn);
  SocketData* CreateSocketDataEntry(int sfd);
  void ModifySocketDataEntry(int sfd, int newsfd);
  void DeleteSocketDataEntry(int sfd);
  int  InsertSocketData(int sfd, SocketMessage msg); 

  void ProcessMsg(SocketMessage msg);
  std::queue<SocketMessage> msgQueue;

 public:
  static SocketInterface* GetInstance(); // singleton pattern
 
  void InitSocketInterface();
  int  GetListenSocket();

  void PushToQueue(SocketMessage msg) {msgQueue.push(msg);}
  void ProcessQueue();
  void ProcessNetworkEvent();
  void PrintSocketList();
};

std::string GetSerializedString(P2PMessage msg);
P2PMessage  GetDeserializedMsg(std::string str);

#endif // P2P_SOCKET_H
