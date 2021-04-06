//
// Created by ilios on 21. 2. 15..
//

#ifndef BLEEP_MONERO_NODE_PRIMITIVES_H
#define BLEEP_MONERO_NODE_PRIMITIVES_H

#include <ev++.h>
#include <memory>
#include <chrono>
#include <ipdb/IPDatabase.h>
#include <node/Node.h>

#include <utility/AttackStat.h>
#include <p2p/net_peerlist.h>

namespace sybiltest {


    class MoneroNodePrimitives {
    private:
        // to inform attack results to the AttackStat object
        AttackStat *_attackStat;
        bool _informed = false;
    private:
        // to deal with entire IP database
        IPDatabase *_ipdb;
    private:
        // to deal with peerlist_entry
        std::vector<nodetool::peerlist_entry> _addr_ip_list;

    protected:
        std::string _myIP;
        NodeType _type;
        std::map<int, TCPControl> _mTCPControl;
        std::string _targetIP = "";
        int _targetPort = -1;
    private:
        // Monero-specific primitives
        std::chrono::system_clock::time_point _setupTime;

    protected:
        // This function is only for shadow active node
        virtual void tryReconnectToTarget() {};

    public:
        void SetTarget(std::string targetIP, int targetPort) {
            _targetIP = targetIP;
            _targetPort = targetPort;
        }

        std::string GetIP() { return _myIP; }

        TCPControl &GetTCPControl(int fd) { return _mTCPControl[fd]; }

        void NewTCPControl(int fd) {
            assert(_mTCPControl.find(fd) == _mTCPControl.end());
            _mTCPControl.try_emplace(fd);
        }

        void RemoveTCPControl(int fd) { _mTCPControl.erase(fd); }

        void SendMsg(int data_fd, std::string msg) {
            TCPControl &tcpBuffer = _mTCPControl[data_fd];
            tcpBuffer.PushBackSendBuffer(std::vector<unsigned char>(msg.begin(), msg.end()));
        }

        void SendMsg(int data_fd, int value) {
            TCPControl &tcpBuffer = _mTCPControl[data_fd];
            const char *value_char = (const char *) &value;
            std::vector<unsigned char> charvec(value_char, value_char + sizeof(int));
            tcpBuffer.PushBackSendBuffer(charvec);
        }

        void SendMsg(int data_fd, std::vector<unsigned char> &msg) {
            TCPControl &tcpBuffer = _mTCPControl[data_fd];
            tcpBuffer.PushBackSendBuffer(msg);
        }

        void SendMsgUsingMsgHdr(int data_fd, std::string msgstr) {
            struct msghdr msg;
            struct iovec iov;

            iov.iov_base = (void *) msgstr.c_str();
            iov.iov_len = msgstr.size();

            msg.msg_name = NULL;
            msg.msg_namelen = 0;
            msg.msg_iov = &iov;
            msg.msg_iovlen = 1;
            msg.msg_control = NULL;
            msg.msg_controllen = 0;

            // int sendmsg_flag = MSG_NOSIGNAL; // ignore flag (shadow does not support it)
            SendMsg(data_fd, &msg);
        }

        void SendMsg(int data_fd, struct msghdr *message) {
            if (message->msg_name != NULL) {
                std::cout << "sendmsg with msg_name is not supported" << "\n";
                exit(-1);
            } else if (message->msg_control != NULL) {
                std::cout << "sendmsg with msg_control is not supported" << "\n";
                exit(-1);
            }

            int i = 0;
            size_t totalIOLength = 0;
            for (i = 0; i < message->msg_iovlen; i++) {
                totalIOLength += message->msg_iov[i].iov_len;
            }

            if (totalIOLength == 0) {
                return;
            } else {
                /* get a temporary buffer and write to it */
                std::vector<unsigned char> charvec;

                for (i = 0; i < message->msg_iovlen; i++) {
                    const unsigned char *buf = (const unsigned char *) message->msg_iov[i].iov_base;
                    charvec.insert(charvec.end(), buf, buf + message->msg_iov[i].iov_len);
                }

                if (charvec.size() > 0) {
                    /* try to write all of the bytes we got from the iov buffers */
                    TCPControl &tcpBuffer = _mTCPControl[data_fd];
                    tcpBuffer.PushBackSendBuffer(charvec);
                }
            }
        }

    public:
        MoneroNodePrimitives(AttackStat *stat, IPDatabase *ipdb, std::string ip, NodeType type) : _attackStat(stat),
                                                                                                  _ipdb(ipdb),
                                                                                                  _myIP(ip),
                                                                                                  _type(type),
                                                                                                  _setupTime(
                                                                                                          std::chrono::system_clock::now()) {}

        void OpAfterConnect(int conn_fd);

        void OpAfterConnected(int data_fd); // called when the socket connection is established by peer

        void OpAfterRecv(int data_fd, std::string recv_str);

        void OpAfterDisconnect();

        void OpAddrInjectionTimeout(std::chrono::system_clock::duration preparePhaseDuration, int periodLength,
                                    double ipPerSec, double shadowRate);
    };
}

#endif //BLEEP_MONERO_NODE_PRIMITIVES_H
