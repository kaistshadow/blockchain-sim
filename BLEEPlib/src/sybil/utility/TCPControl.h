//
// Created by ilios on 21. 2. 18..
//

#ifndef BLEEP_TCPCONTROL_H
#define BLEEP_TCPCONTROL_H

#include <deque>
#include <vector>

namespace libBLEEP_sybil {
    class TCPControl {
    private:
        int _socketfd = -1;
        size_t _offset = 0;
        ev::io *_watcher = nullptr;
        std::deque<std::vector<unsigned char>> _sendBuffer;
        std::string _recvBuffer;
    public:
        TCPControl(int fd, ev::io *w) : _socketfd(fd), _watcher(w) {}

        TCPControl() = default;

        // move constructor
        TCPControl(TCPControl &&other) = default;

        // send buffer management
        const std::vector<unsigned char> &FrontSendBuffer() {
            return _sendBuffer.front();
        }

        size_t getSendOffset() { return _offset; }

        void setSendOffset(size_t offset) { _offset = offset; }

        bool IsEmptySendBuffer() { return _sendBuffer.empty(); }

        void RemoveFrontSendBuffer() { _sendBuffer.pop_front(); }

        void PushBackSendBuffer(std::vector<unsigned char> data) { _sendBuffer.push_back(data); }

        // recv buffer management
        void AppendToRecvBuffer(std::string str) { _recvBuffer.append(str); }

        std::string &GetRecvBuffer() { return _recvBuffer; }

        void SetRecvBuffer(std::string newstr) { _recvBuffer = newstr; }


        // watcher management
        void SetWrite() { _watcher->set(_socketfd, ev::READ | ev::WRITE); }

        void UnsetWrite() { _watcher->set(_socketfd, ev::READ); }
    };
}

#endif //BLEEP_TCPCONTROL_H
