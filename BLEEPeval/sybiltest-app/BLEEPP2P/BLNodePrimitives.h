//
// Created by ilios on 21. 2. 15..
//

#ifndef BLEEP_BL_NODE_PRIMITIVES_H
#define BLEEP_BL_NODE_PRIMITIVES_H

#include <ev++.h>
#include <memory>
#include "node/Node.h"

#include "utility/AttackStat.h"

namespace sybiltest {


    class BLNodePrimitives {
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
    protected:
        // This function is only for shadow active node
        virtual void tryReconnectToTarget() {};

    public:
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

    public:
        BLNodePrimitives(AttackStat *stat, IPDatabase *ipdb, std::string ip, NodeType type) : _attackStat(stat),
                                                                                              _ipdb(ipdb), _myIP(ip),
                                                                                              _type(type) {}

        void OpAfterConnect(int conn_fd);

        void OpAfterConnected(int data_fd); // called when the socket connection is established by peer

        void OpAfterRecv(int data_fd, std::string recv_str);

        void OpAfterDisconnect();
    };
}

#endif //BLEEP_BL_NODE_PRIMITIVES_H
