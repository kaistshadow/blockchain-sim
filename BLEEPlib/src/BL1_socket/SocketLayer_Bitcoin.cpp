// "Copyright [2021] <kaistshadow>"
//
// Created by ilios on 20. 8. 26..
//

#include <string>
#include <memory>
#include <unistd.h>
#include <fcntl.h> /* Added for the nonblocking socket */
#include <arpa/inet.h>
#include <iostream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/serialization/export.hpp>

#include "SocketLayer_Bitcoin.h"
#include "../utility/Assert.h"
#include "../BL2_peer_connectivity/Message.h"

using namespace libBLEEP_BL;

BL_SocketLayer_Bitcoin::BL_SocketLayer_Bitcoin() {
}

void BL_SocketLayer_Bitcoin::RegisterInstance() {
    _instance = new BL_SocketLayer_Bitcoin;
}

BL_SocketLayer_Bitcoin *BL_SocketLayer_Bitcoin::Instance() {
    BL_SocketLayer_Bitcoin* ptr = dynamic_cast<BL_SocketLayer_Bitcoin*>(_instance);
    libBLEEP::M_Assert(ptr != nullptr, "dynamic cast failed");
    return ptr;
}

void BL_SocketLayer_Bitcoin::RecvHandler(int fd) {
    std::cout << "DoRecv!" << "\n";

    // recv from socket, and append received data into the buffer.
    char string_read[2001];
    int n;
    memset(string_read, 0, 2000);
    while (1) {
        n = recv(fd, string_read, 2000, 0);
        if (n == 2000) {
            _recvBuffManager.AppendToBuffer(fd, string_read, n);
            std::cout << "received data:[" << string_read << "]" << "\n";
            memset(string_read, 0, 2000);
            continue;
        } else if (0 < n) {
            _recvBuffManager.AppendToBuffer(fd, string_read, n);
            std::cout << "received data:[" << string_read << "]" << "\n";
            break;
        } else if (n == 0) {
            std::cout << "connection closed while recv" << "\n";
            // TODO : notify socketClose event?
            AsyncEvent event(AsyncEventEnum::PeerSocketClose);
            event.GetData().SetClosedSocket(_socketManager.GetDataSocket(fd));
            MainEventManager::Instance()->PushAsyncEvent(event);

            _socketManager.RemoveDataSocket(fd);
            _recvBuffManager.RemoveRecvBuffer(fd);
            break;
        } else if (n < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                std::cout << "recv failed errno=" << errno << strerror(errno) << "\n";
                exit(-1);
            }
            break;
        }
    }

    // TODO : parsing a msg
    std::shared_ptr<SocketRecvBuffer> recvBuffer =  _recvBuffManager.GetRecvBuffer(fd);
    if (recvBuffer) {
        // strncmp(str, substr, strlen(substr)) will return 0 if str starts with substr.
        // Check if the received string starts with the bleep magic value.

        while (true) {
            const char *recvBuf = recvBuffer->recv_str.c_str();
            if (!strncmp(recvBuf, BITCOIN_MAGIC, BITCOIN_MAGIC_SIZE)) {
                std::cout << "bitcoin magic received" << "\n";

                // retrieve the size of the msg if possible
                uint32_t payload_size = 0;
                if (recvBuffer->recv_str.size() >= BITCOIN_MAGIC_SIZE + BITCOIN_COMMAND_SIZE + sizeof(uint32_t)) {
                    memcpy(&payload_size, recvBuf + BITCOIN_MAGIC_SIZE + BITCOIN_COMMAND_SIZE, sizeof(uint32_t));
                    std::cout << "msg length received : " << payload_size << "\n";
                } else {
                    break;
                }
                // recv entire msg if possible
                if (payload_size && recvBuffer->recv_str.size() >= BITCOIN_MAGIC_SIZE + BITCOIN_COMMAND_SIZE + sizeof(uint32_t) * 2 + payload_size) {
                    std::string command(recvBuf+BITCOIN_MAGIC_SIZE, BITCOIN_COMMAND_SIZE);
                    uint32_t checksum;
                    memcpy(&checksum, recvBuf + BITCOIN_MAGIC_SIZE + BITCOIN_COMMAND_SIZE + sizeof(uint32_t), sizeof(uint32_t));
                    std::string payload( recvBuf + BITCOIN_MAGIC_SIZE + BITCOIN_COMMAND_SIZE  + sizeof(uint32_t)*2, payload_size);

//                    std::cout << "printing received payload" << "\n";
//                    std::cout << "[";
//                    for (size_t i = 0; i < payload_size; i++) {
//                        std::cout << std::setfill('0') << std::setw(2) << std::hex << (0xff & (unsigned int)(payload[i]));
//                    }
//                    std::cout << "]" << "\n";

                    AsyncEvent event(AsyncEventEnum::BitcoinRecvMsg);
                    event.GetData().SetBitcoinRecvSocket(fd);
                    event.GetData().SetBitcoinCommand(command);
                    event.GetData().SetBitcoinPayload(payload);
                    event.GetData().SetBitcoinPayloadLen(payload_size);
                    event.GetData().SetBitcoinPayloadChecksum(checksum);
                    MainEventManager::Instance()->PushAsyncEvent(event);

                    // TODO : recvBuffer should be updated efficiently. (minimizing a duplication)
                    std::string remain = recvBuffer->recv_str.substr(BITCOIN_MAGIC_SIZE + BITCOIN_COMMAND_SIZE + sizeof(uint32_t)*2 + payload_size);
                    recvBuffer->recv_str = remain;
                } else if (payload_size == 0 && recvBuffer->recv_str.size() >= BITCOIN_MAGIC_SIZE + BITCOIN_COMMAND_SIZE + sizeof(uint32_t) * 2) {
                    std::string command(recvBuf+BITCOIN_MAGIC_SIZE, BITCOIN_COMMAND_SIZE);
                    uint32_t checksum;
                    memcpy(&checksum, recvBuf + BITCOIN_MAGIC_SIZE + BITCOIN_COMMAND_SIZE + sizeof(uint32_t), sizeof(uint32_t));

                    AsyncEvent event(AsyncEventEnum::BitcoinRecvMsg);
                    event.GetData().SetBitcoinRecvSocket(fd);
                    event.GetData().SetBitcoinCommand(command);
                    event.GetData().SetBitcoinPayloadLen(payload_size);
                    event.GetData().SetBitcoinPayloadChecksum(checksum);
                    MainEventManager::Instance()->PushAsyncEvent(event);

                    // TODO : recvBuffer should be updated efficiently. (minimizing a duplication)
                    std::string remain = recvBuffer->recv_str.substr(BITCOIN_MAGIC_SIZE + BITCOIN_COMMAND_SIZE + sizeof(uint32_t)*2);
                    recvBuffer->recv_str = remain;
                } else {
                    break;
                }
            } else {
                break;
            }
        }
    }
}

void BL_SocketLayer_Bitcoin::CreateSocketForShadowIP(int port, const char *shadow_ip_addr) {
    _socketManager.CreateListenSocketForShadowIP(port, shadow_ip_addr);
}
