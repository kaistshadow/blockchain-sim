#ifndef HANDLE_NETWORK_H
#define HANDLE_NETWORK_H

#include <ev.h>
#include <memory>
#include "TestMessage.h"

#define HANDLE_NETWORK_TEST 0

class HandleNetwork {

 public:
    virtual ~HandleNetwork() {};
    static std::shared_ptr<HandleNetwork> create(const int type);

    virtual int JoinNetwork() = 0;
    virtual void HandleRecvSocketIO(int fd) = 0;
    virtual void HandleSendSocketIO(int fd) = 0;
    virtual void HandleAcceptSocketIO(int fd) = 0;

    virtual void UnicastMsg(std::string destip, TestMessage& msg) = 0;
};


#endif // HANDLE_NETWORK_H
