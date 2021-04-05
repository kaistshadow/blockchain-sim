//
// Created by csrc on 21. 3. 25..
//

#ifndef BLEEP_MONITORINGNODE_H
#define BLEEP_MONITORINGNODE_H


#include <iostream>
#include <string>
#include <memory>
#include <arpa/inet.h>
#include <fcntl.h> /* Added for the nonblocking socket */
#include <unistd.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <strings.h>

#include "shadow_interface.h"
#include "Node.h"

namespace tpstest {

template<class NodePrimitives>
class MonitoringNode : public Node<NodePrimitives> {
    public:
    MonitoringNode(std::string virtualIp, int listenPort = 0)
      : Node<NodePrimitives>( virtualIp, listenPort, NodeType::MonitoringNode) {
    }


    //variable for monitoring node
    unsigned int mainchain_total_tx_cnt = 0;
    double mainchain_total_time = 0;
    double mainchain_tps;
    typedef typename Node<NodePrimitives>::BlockInfo BlockInfo;
    std::map<std::string, BlockInfo> block_table;
//  tree


  // move constructor
  MonitoringNode(MonitoringNode &&other) = default;

  // API for connection to target
  int tryConnectToTarget(std::string targetIP, int targetPort) {
    // create new socket for a connection
    int conn_fd = CreateNewSocket();

    // bind to shadow's virtual NIC
    struct sockaddr_in new_addr;    /* my address information */
    new_addr.sin_family = AF_INET;         /* host byte order */
    new_addr.sin_port = htons(0);     /* short, network byte order, use random port */
    new_addr.sin_addr.s_addr = inet_addr(Node<NodePrimitives>::_myIP.c_str());
    bzero(&(new_addr.sin_zero), 8);        /* zero the rest of the struct */
    if (bind(conn_fd, (struct sockaddr *) &new_addr, sizeof(struct sockaddr)) == -1) {
      perror("bind");
      exit(1);
    }

    struct sockaddr_in targetaddr;
    bzero(&targetaddr, sizeof(targetaddr));
    targetaddr.sin_family = AF_INET;
    targetaddr.sin_port = htons(targetPort);
    targetaddr.sin_addr.s_addr = inet_addr(targetIP.c_str());

    // call actual systemcall API for connect
    // TODO : error-handling logic should be updated
    int ret = connect(conn_fd, (struct sockaddr *) &targetaddr, sizeof(targetaddr));
    if (ret < 0 && errno != EINPROGRESS) {
      perror("connect");
      std::cout << "Unable to connect to " << targetIP << "\n";
      exit(-1);
    } else if (ret == 0) {
      std::cout << "connection established" << "\n";
      std::cout << "non-blocking socket is immediately established. is it possible?" << "\n";
      exit(-1);
    } else if (ret > 0) {
      std::cout << "unexpected return value of non-blocking connect" << "\n";
      exit(-1);
    }

    // assign an io event watcher for (connect) tried socket descriptor
    // and register an event watcher to monitor for the beginning of I/O operation (A.K.A reactor pattern)
    auto[it, result] = Node<NodePrimitives>::_mConnSocketWatcher.try_emplace(conn_fd);

    if (result) {
      ev::io &watcher = it->second;
      watcher.set<Node<NodePrimitives>, &Node<NodePrimitives>::conncb>(this);
      watcher.set(conn_fd, ev::WRITE);
      watcher.start();
    } else {
      std::cout<<"socket watch result is null\n";
    }
    Node<NodePrimitives>::_targetIP = targetIP;
    Node<NodePrimitives>::_targetPort = targetPort;

    return conn_fd;
  }

 private:

 private:

  int CreateNewSocket() {
    // Create a TCP socket in the address family of the specified service.
    unsigned int hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hSocket == -1)
      return -1;

    // Set the no-delay option (disable Nagle's algorithm) on the TCP socket.
    int set = 1;
    setsockopt(hSocket, IPPROTO_TCP, TCP_NODELAY, (const char *) &set, sizeof(int));

    // Set the non-blocking option on the socket.
    if (!SetSocketNonBlocking(hSocket, true)) {
      CloseSocket(hSocket);
      std::cout << "Setting socket to non-blocking failed" << "\n";
      exit(-1);
    }

    return hSocket;
  }

  bool SetSocketNonBlocking(const unsigned int &hSocket, bool fNonBlocking) {
    if (fNonBlocking) {
      int fFlags = fcntl(hSocket, F_GETFL, 0);
      if (fcntl(hSocket, F_SETFL, fFlags | O_NONBLOCK) == -1) {
        return false;
      }
    } else {
      int fFlags = fcntl(hSocket, F_GETFL, 0);
      if (fcntl(hSocket, F_SETFL, fFlags & ~O_NONBLOCK) == -1) {
        return false;
      }
    }

    return true;
  }

  bool CloseSocket(unsigned int &hSocket) {
    if (hSocket == -1)
      return false;
    int ret = close(hSocket);
    if (ret) {
      std::cout << "Socket close failed: " << hSocket << "\n";
      exit(-1);
    }
    hSocket = -1;
    return ret != -1;
  }

    void UpdateTxCnt(unsigned int block_tx) {
      mainchain_total_tx_cnt += block_tx;
    }

    void UpdateTPSTime(unsigned int blocktime) {
        mainchain_total_time += blocktime;
    }

    void UpdateTPS(unsigned int txcnt, unsigned int time) {
        UpdateTxCnt(txcnt);
        UpdateTPSTime(time);
        mainchain_tps = mainchain_total_tx_cnt/mainchain_total_time;
        std::cout<<"updateTPS : "<<mainchain_tps<<" tx = "<<mainchain_total_tx_cnt<<" time = "<<mainchain_total_time<<"\n";
    }
     bool RegisterBlock(BlockInfo  newblock) {
      auto result = block_table.emplace(newblock.blockhash, newblock);
      if(!result.second) {
          std::cout << "blockhash " << newblock.blockhash << " is already exist in block_table!\n";\
          return false;
      }

      if(mainchain_total_tx_cnt == 0) {
          UpdateTPS(newblock.txcount,0);
          return true;
      }
      int32_t prevtimestamp = block_table[newblock.prevblockhash].timestamp;
      UpdateTPS(newblock.txcount, newblock.timestamp - prevtimestamp);
      return true;

  }

 private:

 public:
};
}

#endif //BLEEP_MONITORING_NODE
