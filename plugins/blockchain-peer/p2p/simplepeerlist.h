#ifndef P2P_SIMPLEPEERLIST_H
#define P2P_SIMPLEPEERLIST_H

#include <vector>
#include <string>
#include "socketmessage.h"

#define ActiveSize 4
#define PassiveSize 30

class Peer{ 
 public:
  Peer(std::string pn, int fd) {peername=pn;sfd=fd;}
  std::string peername;
  int sfd;
};
typedef std::vector<Peer> PeerList;

class SimplePeerList {
 private:
  SimplePeerList() { 
    active_view  = std::vector<Peer>();
    passive_view = std::vector<Peer>();
  };                                    // singleton pattern
  static SimplePeerList* instance;      // singleton pattern
    
 public:
  static SimplePeerList* GetInstance(); // sigleton pattern

  PeerList active_view;
  PeerList passive_view;
  
  void  InitPeerList(int argc, char *argv[]);
  Peer* FindPeerFromAll(int fd);
  
  void DropRandomFromActive();
  void DropFromActive(int fd);
  int  ExistInActive(int fd);
  void AddToActive(Peer node);

  void DropRandomFromPassive();
  void DropFromPassive(int fd);
  int  ExistInPassiveById(std::string pn);
  int  ExistInPassive(int fd);
  void AddToPassive(Peer node);

  void PrintActive();
  void PrintPassive();
};

#endif // P2P_SIMPLEPEERLIST_H
