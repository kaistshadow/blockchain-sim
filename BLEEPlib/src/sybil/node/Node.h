//
// Created by ilios on 21. 2. 17..
//

#ifndef BLEEP_NODE_H
#define BLEEP_NODE_H

#include <map>
#include <assert.h>
#include "../utility/TCPControl.h"

namespace libBLEEP_sybil {
    class Node {
    protected:
        std::string _myIP;
        std::map<int, TCPControl> _mTCPControl;
    public:
        Node(std::string vIP) : _myIP(vIP) {}

        TCPControl &GetTCPControl(int fd) { return _mTCPControl[fd]; }

        void NewTCPControl(int fd, ev::io *watcher) {
            assert(_mTCPControl.find(fd) == _mTCPControl.end());
            _mTCPControl.try_emplace(fd, fd, watcher);
        }

        void RemoveTCPControl(int fd) { _mTCPControl.erase(fd); }

        void SendMsg(int data_fd, std::string msg) {
            TCPControl &tcpBuffer = _mTCPControl[data_fd];
            tcpBuffer.PushBackSendBuffer(std::vector<unsigned char>(msg.begin(), msg.end()));
            tcpBuffer.SetWrite();
        }

        void SendMsg(int data_fd, int value) {
            TCPControl &tcpBuffer = _mTCPControl[data_fd];
            const char *value_char = (const char *) &value;
            std::vector<unsigned char> charvec(value_char, value_char + sizeof(int));
            tcpBuffer.PushBackSendBuffer(charvec);
            tcpBuffer.SetWrite();
        }

        virtual std::string GetIP() = 0;
    };
}

#endif //BLEEP_NODE_H
