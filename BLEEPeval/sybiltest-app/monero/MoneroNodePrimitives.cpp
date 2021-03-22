//
// Created by ilios on 21. 2. 15..
//
#include <iostream>
#include "MoneroNodePrimitives.h"
#include <random>
#include <algorithm>

// monero-specific data management

using namespace sybiltest;
using namespace std;

void MoneroNodePrimitives::OpAfterConnect(int conn_fd) {
    cout << "OpAfterConnect for node [" << GetIP() << "]" << "\n";
    switch (_type) {
        case NodeType::Attacker: {
            // send initializing version message
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
        // first, dump a message header

        // second, dump a message

        // third, parse a message

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