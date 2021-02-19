//
// Created by ilios on 21. 2. 15..
//

#ifndef BLEEP_ASYNC_CALLBACKS_H
#define BLEEP_ASYNC_CALLBACKS_H

#include <ev++.h>
#include <memory>
#include "../node/Node.h"

namespace libBLEEP_sybil {
    class BLNodePrimitives {
    protected:
        std::string _myIP;
        NodeType _type;
        std::map<int, TCPControl> _mTCPControl;
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
        BLNodePrimitives(std::string ip, NodeType type) : _myIP(ip), _type(type) {}

        void OpAfterConnect(int conn_fd);

        void OpAfterRecv(int data_fd, std::string recv_str);

    };
}

#endif //ASYNC_CALLBACKS_H
