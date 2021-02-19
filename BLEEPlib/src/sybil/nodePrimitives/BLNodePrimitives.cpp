//
// Created by ilios on 21. 2. 15..
//
#include <iostream>
#include "BLNodePrimitives.h"

#include "BL2_peer_connectivity/Message.h"
#include "BL2_peer_connectivity/Peer.h"
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/archive/binary_iarchive.hpp>


using namespace libBLEEP_sybil;

void BLNodePrimitives::OpAfterConnect(int conn_fd) {
    std::cout << "OpAfterConnect for node [" << GetIP() << "]" << "\n";
    switch (_type) {
        case NodeType::Benign: {
            // send initializing message
            std::shared_ptr<libBLEEP_BL::Message> peerIDmsg = std::make_shared<libBLEEP_BL::Message>(
                    libBLEEP_BL::PeerId(GetIP()), libBLEEP_BL::PeerId(""), "notifyPeerId");

            // serialize message obj into an std::string
            std::string serial_str;
            boost::iostreams::back_insert_device<std::string> inserter(serial_str);
            boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
            boost::archive::binary_oarchive oa(s);
            oa << peerIDmsg;
            s.flush();

            SendMsg(conn_fd, BLEEP_MAGIC);
            SendMsg(conn_fd, serial_str.size());
            SendMsg(conn_fd, serial_str);
            // how to send P2P message -> use BLEEP library logic
            // how to manage Transport layer buffer -> TCPControl internally manages buffer, ReactorWatcher refers TCPControl
            // how to manage asynchronous I/O events -> TCPControl controls watcher
            break;
        }
        case NodeType::Shadow:
            break;
    }
}

void BLNodePrimitives::OpAfterRecv(int data_fd, std::string recv_str) {
    switch (_type) {
        case NodeType::Benign: {
            // recv to RecvBuffer
            TCPControl &tcpControl = GetTCPControl(data_fd);
            tcpControl.AppendToRecvBuffer(recv_str);

            // parsing a received data stream
            std::string &recvbufstr = tcpControl.GetRecvBuffer();
            while (true) {
                const char *recvBuf = recvbufstr.c_str();
                if (!strncmp(recvBuf, BLEEP_MAGIC, BLEEP_MAGIC_SIZE)) {
                    // bleep magic received
                    // retrieve the size of the msg if possible
                    int msg_size = 0;
                    if (recvbufstr.size() >= BLEEP_MAGIC_SIZE + sizeof(int)) {
                        memcpy(&msg_size, recvBuf + BLEEP_MAGIC_SIZE, sizeof(int));
                        // msg length received
                    } else
                        break;

                    // recv entire msg if possible
                    if (msg_size && recvbufstr.size() >= BLEEP_MAGIC_SIZE + sizeof(int) + msg_size) {
                        // start deserializing MSG
                        recvBuf += BLEEP_MAGIC_SIZE + sizeof(int);
                        std::shared_ptr<libBLEEP_BL::Message> msg;
                        boost::iostreams::basic_array_source<char> device(recvBuf, msg_size);
                        boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
                        boost::archive::binary_iarchive ia(s);
                        ia >> msg;

                        // deserializing MSG complete
                        std::cout << "OpAfterRecv: deserializing MSG complete, MSG type:" << msg->GetType() << "\n";

                        if (msg->GetType() == "PING") {
                            // send PONG message
                            std::shared_ptr<libBLEEP_BL::Message> pongMsg = std::make_shared<libBLEEP_BL::Message>(
                                    libBLEEP_BL::PeerId(GetIP()), libBLEEP_BL::PeerId(""), "PONG");

                            // serialize message obj into an std::string
                            std::string serial_str;
                            boost::iostreams::back_insert_device<std::string> inserter(serial_str);
                            boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > outs(inserter);
                            boost::archive::binary_oarchive oa(outs);
                            oa << pongMsg;
                            outs.flush();

                            SendMsg(data_fd, BLEEP_MAGIC);
                            SendMsg(data_fd, serial_str.size());
                            SendMsg(data_fd, serial_str);
                        }

                        // Maybe, recvBuffer can be updated efficiently. (minimizing a duplication)
                        std::string remain = recvbufstr.substr(BLEEP_MAGIC_SIZE + sizeof(int) + msg_size);
                        tcpControl.SetRecvBuffer(remain);
                    } else
                        break;
                } else
                    break;
            }
            break;
        }
        case NodeType::Shadow:
            break;
    }
}

