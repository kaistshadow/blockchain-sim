//
// Created by ilios on 20. 10. 06..
//

#include <ev++.h>

#include <algorithm>
#include <string_view>
#include <iostream>
#include <deque>

#include "shadow_interface.h"
#include <arpa/inet.h>
#include <fcntl.h> /* Added for the nonblocking socket */
#include <memory>

#include <util/system.h>
#include <bitcoind.h>
#include <net.h>
#include <net_processing.h>


std::string string_to_hex(const std::string& input)
{
    static const char hex_digits[] = "0123456789ABCDEF";

    std::string output;
    output.reserve(input.length() * 2);
    for (unsigned char c : input)
    {
        output.push_back(hex_digits[c >> 4]);
        output.push_back(hex_digits[c & 15]);
    }
    return output;
}
// Convert string of hex numbers to its equivalent char-stream
std::string hex_to_string(const std::string& hexstr)
{
    std::string output;
    output.resize((hexstr.size() + 1) / 2);

    for (size_t i = 0, j = 0; i < output.size(); i++, j++)
    {
        output[i] = (hexstr[j] & '@' ? hexstr[j] + 9 : hexstr[j]) << 4, j++;
        output[i] |= (hexstr[j] & '@' ? hexstr[j] + 9 : hexstr[j]) & 0xF;
    }
    return output;
}


class RecvMsgManager {
private:
    std::map<int, std::list<CNetMessage> > _RecvMsgMap; // map fd -> list<CNetMessage> structure
    std::map<int, std::list<CNetMessage> > _ProcessMsgMap; // map fd -> list<CNetMessage> structure
public:
    std::list<CNetMessage>& GetRecvMsg(int fd) {
        return _RecvMsgMap[fd];
    }
    std::list<CNetMessage>& GetProcessMsg(int fd) {
        return _ProcessMsgMap[fd];
    }
};

class SendMsgManager {
private:
    std::map<int, std::deque<std::vector<unsigned char>> > _SendMsgMap; // map fd -> std::deque<std::vector<unsigned char>>
    std::map<int, size_t> _SendOffsetMap; //map fd -> size_t
    std::map<int, CAddress> _addrMap; // map fd -> CAddress

public:
    std::deque<std::vector<unsigned char>>& GetSendMsg(int fd) {
        return _SendMsgMap[fd];
    }
    void SetAddrMap(int fd, sockaddr_in their_addr) {
        CAddress addr;
        if (!addr.SetSockAddr((const struct sockaddr*)&their_addr)) {
            LogPrintf("Warning: Unknown socket family\n");
        }
        _addrMap[fd] = addr;
    }
    CAddress& GetAddr(int fd) {
        return _addrMap[fd];
    }
    size_t GetSendOffset(int fd) {
        return _SendOffsetMap[fd];
    }
    void SetSendOffset(int fd, size_t offset) {
        _SendOffsetMap[fd] = offset;
    }
};

RecvMsgManager recvMsgManager;
SendMsgManager sendMsgManager;

class DataSocketWatcher {
private:
    std::string _shadow_ip;
    int _fd;
    /* event watcher */
    ev::io _watcher;

    /* event io callback */
    void _dataSocketIOCallback(ev::io &w, int revents) {
        std::cout << "data socket IO callback called!" << "\n";

        if (revents & EV_READ) {
            std::cout << "DoRecv!" << "\n";

            // recv from socket, and append received data into the buffer.
            // typical socket buffer is 8K-64K
            char pchBuf[0x10000];

            int nBytes = 0;

            nBytes = recv(_fd, pchBuf, sizeof(pchBuf), MSG_DONTWAIT); // What is the meaning of MSG_DONTWAIT?
            if (nBytes > 0) {
                std::string recv_str(pchBuf, nBytes);
                std::cout << "received data at " << _shadow_ip << ":[" << recv_str << "]" << "\n";
                std::cout << "received data hex:[" << string_to_hex(recv_str) << "]" << "\n";

                bool ret = BitcoinLibReceiveMsg(pchBuf, nBytes, recvMsgManager.GetRecvMsg(_fd), recvMsgManager.GetProcessMsg(_fd));
                if (!ret) {
                    std::cout << "error while BitcoinLibReceiveMsg" << "\n";
                    exit(-1);
                }
                std::list<CNetMessage>& vProcessMsg = recvMsgManager.GetProcessMsg(_fd);
                if (!vProcessMsg.empty()) {
                    std::list<CNetMessage> msgs;

                    // Just take one message
                    msgs.splice(msgs.begin(), vProcessMsg, vProcessMsg.begin());
                    CNetMessage& msg(msgs.front());

                    std::cout << msg.hdr.GetCommand() << "\n";

                    const unsigned char MessageStartChars[4] = {'\v', '\021', '\t', '\a'};

                    if (memcmp(msg.hdr.pchMessageStart, MessageStartChars, CMessageHeader::MESSAGE_START_SIZE) != 0) {
                        std::cout << "INVALID MESSAGESTART " << msg.hdr.GetCommand() << "\n";
                        exit(-1);
                    }

                    // Read header
                    CMessageHeader& hdr = msg.hdr;
                    if (!hdr.IsValid(MessageStartChars))
                    {
                        LogPrint(BCLog::NET, "PROCESSMESSAGE: ERRORS IN HEADER %s\n", hdr.GetCommand());
                        exit(-1);
                    }

                    ret = BitcoinLibProcessMessage(msg, MessageStartChars, sendMsgManager.GetSendMsg(_fd), sendMsgManager.GetAddr(_fd));
                    if (!ret)
                    {
                        std::cout << "error while processing message" << "\n";
                        exit(-1);
                    }
                    if (!sendMsgManager.GetSendMsg(_fd).empty())
                        SetWritable(); // make watcher to monitor write event
                }

            } else if (nBytes == 0) {
                std::cout << "connection closed while recv" << "\n";
                delete this; // is it okay for this self-destruction?
            } else if (nBytes < 0) {
                // error
                std::cout << "Error while recv" << "\n";
            }
        }
        else if (revents & EV_WRITE) {
            std::deque<std::vector<unsigned char>>& vSendMsg = sendMsgManager.GetSendMsg(_fd);
            auto it = vSendMsg.begin();
            size_t nSentSize = 0;

            size_t nSendOffset = sendMsgManager.GetSendOffset(_fd);

            while (it != vSendMsg.end()) {
                const auto &data = *it;
                assert(data.size() > nSendOffset);
                int nBytes = 0;
                {
                    nBytes = send(_fd, reinterpret_cast<const char*>(data.data()) + nSendOffset, data.size() - nSendOffset, MSG_NOSIGNAL | MSG_DONTWAIT);
                }
                if (nBytes > 0) {
                    nSendOffset += nBytes;
                    nSentSize += nBytes;
                    if (nSendOffset == data.size()) {
                        nSendOffset = 0;
                        it++;
                    } else {
                        // could not send full message; stop sending more
                        break;
                    }
                } else {
                    if (nBytes < 0) {
                        // error
                        int nErr = WSAGetLastError();
                        if (nErr != WSAEWOULDBLOCK && nErr != WSAEMSGSIZE && nErr != WSAEINTR && nErr != WSAEINPROGRESS)
                        {
                            LogPrintf("socket send error %s\n");
                            exit(-1);
                        }
                    }
                    // couldn't send anything at all
                    break;
                }
            }

            if (it == vSendMsg.end()) {
                assert(nSendOffset == 0);
                UnsetWritable();
            }
            vSendMsg.erase(vSendMsg.begin(), it);
        }
    }
public:
    DataSocketWatcher(int fd) {
        _watcher.set<DataSocketWatcher, &DataSocketWatcher::_dataSocketIOCallback> (this);
        _watcher.start(fd, ev::READ);
        _fd = fd;
    }
    DataSocketWatcher(int fd, std::string hostname) {
        _watcher.set<DataSocketWatcher, &DataSocketWatcher::_dataSocketIOCallback> (this);
        _watcher.start(fd, ev::READ);
        _fd = fd;
        _shadow_ip = hostname;
    }
    void SetWritable() {
        _watcher.set(_fd, ev::READ | ev::WRITE);
    }
    void UnsetWritable() {
        _watcher.set(_fd, ev::READ);
    }
};

class ListenSocketWatcher {
private:
    ev::io listen_watcher;
    std::string _shadow_ip;
    void _listenSocketIOCallback (ev::io &w, int revents) {
        std::cout << "listen socket IO callback called!" << "\n";
        struct 	sockaddr_in 	their_addr; /* connector's address information */
        int sock_fd;
        socklen_t 			sin_size;

        sin_size = sizeof(struct sockaddr_in);
        sock_fd = accept(w.fd, (struct sockaddr *)&their_addr, &sin_size);
        if ( sock_fd != -1 ) {
            std::cout << "server: got connection from " << inet_ntoa(their_addr.sin_addr) << "\n";
            fcntl(sock_fd, F_SETFL, O_NONBLOCK);

            // create event watcher for the DataSocket
            new DataSocketWatcher(sock_fd, _shadow_ip);

            sendMsgManager.SetAddrMap(sock_fd, their_addr);
        }
        else {
            if( errno != EAGAIN && errno != EWOULDBLOCK ) {
                std::cout << "accept() failed errno=" << errno << strerror(errno) << "\n";
                exit(-1);
            }
        }
    }
public:
    ListenSocketWatcher(char *shadow_ip) {
        _shadow_ip = std::string(shadow_ip);

        int fd = BitcoinLibBindSocket(_shadow_ip, 18333);

        if (listen(fd, 10) == -1) {
            perror("listen");
            exit(1);
        }

        listen_watcher.set<ListenSocketWatcher, &ListenSocketWatcher::_listenSocketIOCallback> (this);
        listen_watcher.start(fd, ev::READ);
    }
};



int main(int argc, char *argv[]) {

    TestHelloBitcoinLib();

    exported_main();

    std::cout << "Starting ISP-server for emulated benign nodes" << "\n";

    puts_temp("test shadow_interface\n");

    struct ev_loop *libev_loop = EV_DEFAULT;
    ListenSocketWatcher listenSocketWatcher("1.2.0.1");

    //ListenSocketWatcher listenSocketWatcher2("11.0.0.11");

    while (true) {
        std::cout << "before ev_run" << "\n";
        ev_run (libev_loop, EVRUN_ONCE);
    }
}
