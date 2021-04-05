//
// Created by ilios on 21. 2. 15..
//
#include <iostream>
#include "MoneroNodePrimitives.h"
#include <random>
#include <algorithm>

// monero-specific data management
#include <p2p/net_node.h>
#include "cryptonote_core/cryptonote_core.h"
#include <cryptonote_protocol/cryptonote_protocol_handler.h>
#include <cryptonote_protocol/cryptonote_protocol_handler.inl>
#include <daemon/core.h>
#include <storages/portable_storage_template_helper.h>
#include <net/levin_base.h>

using namespace sybiltest;
using namespace std;

void MoneroNodePrimitives::OpAfterConnect(int conn_fd) {
    cout << "OpAfterConnect for node [" << GetIP() << "]" << "\n";
    switch (_type) {
        case NodeType::Attacker: {
            typedef cryptonote::t_cryptonote_protocol_handler<cryptonote::core> t_protocol_raw;
            typedef nodetool::node_server<t_protocol_raw> t_node_server;
            typedef nodetool::COMMAND_HANDSHAKE_T<cryptonote::CORE_SYNC_DATA>::request t_req;
            t_req arg;
            arg.node_data.peer_id = crypto::rand<uint64_t>();
            arg.node_data.my_port = 38080; // listening port
            arg.node_data.rpc_port = 0;
            arg.node_data.rpc_credits_per_hash = 0;
            memcpy(&arg.node_data.network_id, &::config::testnet::NETWORK_ID, 16);

            arg.payload_data.top_version = 1;
            arg.payload_data.top_id = {72, -54, 124, -45, -56, -34, 91, 106, 77, 83, -46, -122, 31, -67, -82, -36, -95,
                                       65, 85, 53, 89, -7, -66, -107, 32, 6, -128, 83, -51, -88, 67, 11};
            arg.payload_data.current_height = 1;
            arg.payload_data.cumulative_difficulty = 1;
            arg.payload_data.cumulative_difficulty_top64 = 0;
            arg.payload_data.pruning_seed = 0;

            typename epee::serialization::portable_storage stg;
            const_cast<t_req &>(arg).store(stg);
            std::string buff_to_send;
            stg.store_to_binary(buff_to_send);

            uint32_t command = nodetool::COMMAND_HANDSHAKE_T<cryptonote::CORE_SYNC_DATA>::ID;
            bool flag = LEVIN_PACKET_REQUEST;
            bool expect_response = true;
            const epee::levin::bucket_head2 head = epee::levin::make_header(command, buff_to_send.size(), flag,
                                                                            expect_response);

            epee::byte_slice bytes{epee::as_byte_span(head), epee::strspan<uint8_t>(buff_to_send)};

            struct msghdr msg;
            struct iovec iov;

            iov.iov_base = (void *) bytes.data();
            iov.iov_len = bytes.size();

            msg.msg_name = NULL;
            msg.msg_namelen = 0;
            msg.msg_iov = &iov;
            msg.msg_iovlen = 1;
            msg.msg_control = NULL;
            msg.msg_controllen = 0;

            // int sendmsg_flag = MSG_NOSIGNAL; // ignore flag (shadow does not support it
            SendMsg(conn_fd, &msg);


//            boost::program_options::variables_map vm;
//            daemonize::t_core core{vm};
//            t_protocol_raw m_protocol{core.get(), nullptr, false};

            break;
        }
        case NodeType::Benign:
        case NodeType::Shadow: {
            assert(-1);
            break;
        }
    }
}

void MoneroNodePrimitives::OpAfterConnected(int data_fd) {
    // do nothing
}

void MoneroNodePrimitives::OpAfterRecv(int data_fd, string recv_str) {
    // recv to RecvBuffer
    TCPControl &tcpControl = GetTCPControl(data_fd);
    tcpControl.AppendToRecvBuffer(recv_str);

    // get all received data stream
    string &recvbufstr = tcpControl.GetRecvBuffer();

    cout << "opafterRecv" << "\n";

    while (recvbufstr.size() > 0) {
        epee::net_utils::buffer m_cache_in_buffer;
        m_cache_in_buffer.append(recvbufstr.data(), recvbufstr.size());
        if (m_cache_in_buffer.size() < sizeof(epee::levin::bucket_head2)) {
            // header is not fully received
            return;
        }
        epee::levin::bucket_head2 &phead = *(epee::levin::bucket_head2 *) m_cache_in_buffer.span(
                sizeof(epee::levin::bucket_head2)).data();
        int command = phead.m_command;

        if (LEVIN_SIGNATURE != phead.m_signature)
            return;
        int headerReadSize = sizeof(epee::levin::bucket_head2);
        m_cache_in_buffer.erase(headerReadSize);


        if (m_cache_in_buffer.size() < phead.m_cb) {
            // body is not fully received
            return;
        }
        int msgReadSize = phead.m_cb;
        epee::span<const uint8_t> buff_to_invoke = m_cache_in_buffer.carve((std::string::size_type) phead.m_cb);

        if (!(phead.m_flags & (LEVIN_PACKET_REQUEST | LEVIN_PACKET_RESPONSE))) {
            std::cout << "unrecognized monero packet received" << "\n";
            exit(-1);
        }

        bool is_response = phead.m_flags & LEVIN_PACKET_RESPONSE;
        if (is_response) {
            // when the packet is response packet
            // Following commands
            // P2P_COMMANDS_POOL_BASE + 1 : COMMAND_HANDSHAKE_T (1001)
            // P2P_COMMANDS_POOL_BASE + 7 : COMMAND_REQUEST_SUPPORT_FLAGS (1007)
        } else {
            if (phead.m_have_to_return_data) {
                // command_handler->invoke
                // command needs a reply, so we need to handle the command
                // For example, node might receive following commands
                // P2P_COMMANDS_POOL_BASE + 7 : COMMAND_REQUEST_SUPPORT_FLAGS (1007)
                // P2P_COMMANDS_POOL_BASE + 2 : COMMAND_TIMED_SYNC_T (1002)
                std::string return_buff;
                switch (command) {
                    case P2P_COMMANDS_POOL_BASE + 2 : {
                        typedef nodetool::COMMAND_TIMED_SYNC_T<cryptonote::CORE_SYNC_DATA> COMMAND_TIMED_SYNC;
                        typedef COMMAND_TIMED_SYNC::request t_req;
                        typedef COMMAND_TIMED_SYNC::response t_resp;
                        epee::serialization::portable_storage strg;
                        if (!strg.load_from_binary(buff_to_invoke)) {
                            std::cout << "Failed to load_from_binary in command " << command << "\n";
                            exit(-1);
                        }
                        boost::value_initialized<t_req> in_struct;
                        boost::value_initialized<t_resp> out_struct;
                        if (!static_cast<t_req &>(in_struct).load(strg)) {
                            std::cout << "Failed to load in_struct in command " << command << "\n";
                            exit(-1);
                        }

                        // fill response
                        // std::vector<nodetool::peerlist_entry> local_peerlist_new;
                        // local_peerlist_new.reserve(1);
                        // static_cast<t_resp &>(out_struct).local_peerlist_new.
                        t_resp &rsp = static_cast<t_resp &>(out_struct);
                        rsp.payload_data.current_height = 1;
                        rsp.payload_data.top_id = {72, -54, 124, -45, -56, -34, 91, 106, 77, 83, -46, -122, 31, -67,
                                                   -82, -36, -95,
                                                   65, 85, 53, 89, -7, -66, -107, 32, 6, -128, 83, -51, -88, 67, 11};
                        rsp.payload_data.cumulative_difficulty = 1;
                        rsp.payload_data.cumulative_difficulty_top64 = 0;
                        rsp.payload_data.pruning_seed = 0;

                        epee::serialization::portable_storage strg_out;
                        rsp.store(strg_out);
                        // fill response end

                        if (!strg_out.store_to_binary(return_buff)) {
                            std::cout << "Failed to store_to_binary in command " << command << "\n";
                            exit(-1);
                        }

                        epee::levin::bucket_head2 head = epee::levin::make_header(command, return_buff.size(),
                                                                                  LEVIN_PACKET_RESPONSE, false);
                        head.m_return_code = SWAP32LE(1); // return value of invoke function for this command
                        return_buff.insert(0, reinterpret_cast<const char *>(&head), sizeof(head));

                        epee::byte_slice bytes{std::move(return_buff)};
                        // message_data : "\001!\001\001\001\001\001\001\035"
                        // message_size : 62

                        struct msghdr msg;
                        struct iovec iov;

                        iov.iov_base = (void *) bytes.data();
                        iov.iov_len = bytes.size();

                        msg.msg_name = NULL;
                        msg.msg_namelen = 0;
                        msg.msg_iov = &iov;
                        msg.msg_iovlen = 1;
                        msg.msg_control = NULL;
                        msg.msg_controllen = 0;

                        // int sendmsg_flag = MSG_NOSIGNAL; // ignore flag (shadow does not support it)
                        SendMsg(data_fd, &msg);
                        break;
                    }
                    case P2P_COMMANDS_POOL_BASE + 7 : {
                        typedef nodetool::COMMAND_REQUEST_SUPPORT_FLAGS::request t_req;
                        typedef nodetool::COMMAND_REQUEST_SUPPORT_FLAGS::response t_resp;
                        epee::serialization::portable_storage strg;
                        if (!strg.load_from_binary(buff_to_invoke)) {
                            std::cout << "Failed to load_from_binary in command " << command << "\n";
                            exit(-1);
                        }
                        boost::value_initialized<t_req> in_struct;
                        boost::value_initialized<t_resp> out_struct;
                        if (!static_cast<t_req &>(in_struct).load(strg)) {
                            std::cout << "Failed to load in_struct in command " << command << "\n";
                            exit(-1);
                        }
                        // in function handle_get_support_flags, set rsp.support_flags as 1
                        // I don't know the meaning of value 1
                        static_cast<t_resp &>(out_struct).support_flags = 1;

                        epee::serialization::portable_storage strg_out;
                        static_cast<t_resp &>(out_struct).store(strg_out);
                        if (!strg_out.store_to_binary(return_buff)) {
                            std::cout << "Failed to store_to_binary in command " << command << "\n";
                            exit(-1);
                        }

                        epee::levin::bucket_head2 head = epee::levin::make_header(command, return_buff.size(),
                                                                                  LEVIN_PACKET_RESPONSE, false);
                        head.m_return_code = SWAP32LE(1); // return value of invoke function for this command
                        return_buff.insert(0, reinterpret_cast<const char *>(&head), sizeof(head));

                        epee::byte_slice bytes{std::move(return_buff)};
                        // message_data : "\001!\001\001\001\001\001\001\035"
                        // message_size : 62

                        struct msghdr msg;
                        struct iovec iov;

                        iov.iov_base = (void *) bytes.data();
                        iov.iov_len = bytes.size();

                        msg.msg_name = NULL;
                        msg.msg_namelen = 0;
                        msg.msg_iov = &iov;
                        msg.msg_iovlen = 1;
                        msg.msg_control = NULL;
                        msg.msg_controllen = 0;

                        // int sendmsg_flag = MSG_NOSIGNAL; // ignore flag (shadow does not support it)
                        SendMsg(data_fd, &msg);
                        break;
                    }
//                    case nodetool::COMMAND_HANDSHAKE_T<cryptonote::CORE_SYNC_DATA>::ID:
//                    {
//                        typedef nodetool::COMMAND_HANDSHAKE_T<cryptonote::CORE_SYNC_DATA> COMMAND_HANDSHAKE;
//                        typedef COMMAND_HANDSHAKE::request t_req;
//                        typedef COMMAND_HANDSHAKE::response t_resp;
//                        epee::serialization::portable_storage strg;
//                        if (!strg.load_from_binary(buff_to_invoke)) {
//                            std::cout << "Failed to load_from_binary in command " << command << "\n";
//                            exit(-1);
//                        }
//                        boost::value_initialized<t_req> in_struct;
//                        boost::value_initialized<t_resp> out_struct;
//                        if (!static_cast<t_req&>(in_struct).load(strg)) {
//                            std::cout << "Failed to load in_struct in command " << command << "\n";
//                            exit(-1);
//                        }
//
//                        // try_ping? back_ping? at handle_handshake
//                    }
                    default:
                        break;
                }
            } else {
                // packet doesn't need a reply. So ignore it
                // For example, node might receive following commands
                // BC_COMMANDS_POOL_BASE + 10 : NOTIFY_GET_TXPOOL_COMPLEMENT (2010)
            }

        }



//        int command;
//        COMMAND::ID == command
//        typedef nodetool::COMMAND_HANDSHAKE_T<cryptonote::CORE_SYNC_DATA>::request t_req;
//        typedef nodetool::COMMAND_HANDSHAKE_T<cryptonote::CORE_SYNC_DATA>::response t_resp;
//        epee::serialization::portable_storage strg;
//        if(!strg.load_from_binary(epee::strspan<uint8_t>(recvbufstr)))
//        {
//            std::cout << "Failed to load_from_binary" << "\n";
//            return;
//        }
//        typename COMMAND::request, typename COMMAND::response
//        boost::value_initialized<t_in_type> in_struct;
//        boost::value_initialized<t_out_type> out_struct;
//
//        if (!static_cast<t_in_type&>(in_struct).load(strg))
//        {
//            on_levin_traffic(context, false, false, true, in_buff.size(), command);
//            LOG_ERROR("Failed to load in_struct in command " << command);
//            return -1;
//        }
//        on_levin_traffic(context, false, false, false, in_buff.size(), command);


        // first, dump a message header

        // second, dump a message

        // third, parse a message

        // Maybe, recvBuffer can be updated more efficiently. (minimizing a duplication)
        std::string remain = recvbufstr.substr(headerReadSize + msgReadSize);
        tcpControl.SetRecvBuffer(remain);

        // repeating the process until there is remaining data stream in RecvBuffer
        recvbufstr = tcpControl.GetRecvBuffer();
    }
}

void MoneroNodePrimitives::OpAfterDisconnect() {
    // do nothing
}

void MoneroNodePrimitives::OpAddrInjectionTimeout(std::chrono::system_clock::duration preparePhaseDuration,
                                                  int periodLength, double ipPerSec, double shadowRate) {
    auto now = chrono::system_clock::now();
    auto attack_start_time = _setupTime + preparePhaseDuration;

    vector<string> &vReachableIP = _ipdb->GetVReachableIP();
    vector<string> &vUnreachIP = _ipdb->GetVUnreachableIP();
    vector<string> &vShadowIP = _ipdb->GetVShadowIP();

    vector<string> vAddr;

    if (now < attack_start_time) {
        // if it's prepare phase
        int totalIPCount = 1000;
        int legiIPcount = totalIPCount * 0.3;
        int unreLegiIPcount = totalIPCount * 0.7;

    } else {
        // if it's attack phase
        int totalIPCount = std::min(1000, (int) (periodLength * ipPerSec));
        int legiIPcount = totalIPCount * (1 - shadowRate) * 0.3;
        int unreLegiIPcount = totalIPCount * (1 - shadowRate) * 0.7;
        int shadowIPcount = totalIPCount * shadowRate;

    }

    // assume a single data socket
    assert(_mTCPControl.size() == 1);
    int data_fd = _mTCPControl.begin()->first;

    // Create a ADDR message for `vIP`,
    // then push it to message queue
}