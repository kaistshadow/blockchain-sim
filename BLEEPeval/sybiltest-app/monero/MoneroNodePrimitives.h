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

namespace sybiltest {


    class MoneroNodePrimitives {
    private:
        // to inform attack results to the AttackStat object
        AttackStat *_attackStat;
        bool _informed = false;
    private:
        // to deal with entire IP database
        IPDatabase *_ipdb;

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
