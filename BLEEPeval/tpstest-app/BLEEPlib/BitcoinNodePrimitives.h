//
// Created by ilios on 21. 2. 15..
//

#ifndef BLEEP_BITCOIN_NODE_PRIMITIVES_H
#define BLEEP_BITCOIN_NODE_PRIMITIVES_H

#include <ev++.h>
#include <memory>
#include <chrono>
#include <node/Node.h>

#include <key.h>
#include <primitives/transaction.h>
#include <utility/blocks.h>
#include <queue>

#define BLEEP_MAGIC "BLEEPmsg"
#define BLEEP_MAGIC_SIZE 8

namespace tpstest {

    struct spend_data {
        CKey sourceKey;
        CTransaction* sourceTx;
        uint32_t nIn;
    };

    class BitcoinNodePrimitives {
    private:

    private:

    protected:
        std::string _myIP;
        NodeType _type;
        std::map<int, TCPControl> _mTCPControl;
        std::string _targetIP = "";
        int _targetPort = -1;
    private:
        // Bitcoin-specific primitives
        std::chrono::system_clock::time_point _setupTime;

    protected:
        // This function is only for shadow active node
        virtual void tryReconnectToTarget() {};

    public:
        //struct for monitoring node
        struct BlockInfo {
            std::string blockhash;
            std::string prevblockhash;
            uint32_t timestamp;
            unsigned long txcount;
        };


        void SetTarget(std::string targetIP, int targetPort) {
            _targetIP = targetIP;
            _targetPort = targetPort;
        }

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

        void SendMsg(int data_fd, std::vector<unsigned char> &msg) {
            TCPControl &tcpBuffer = _mTCPControl[data_fd];
            tcpBuffer.PushBackSendBuffer(msg);
        }

    public:
        BitcoinNodePrimitives(std::string ip, NodeType type)
                : _myIP(ip), _type(type), _setupTime(std::chrono::system_clock::now()) {}

        void OpAfterConnect(int conn_fd);

        void OpAfterConnected(int data_fd); // called when the socket connection is established by peer

        void OpAfterRecv(int data_fd, std::string recv_str);

        void OpAfterDisconnect();

    private:
        // monitor
        blockforest bf;
        // keygen
        std::queue<spend_data> unspent_keyvalues;
        std::queue<CTransaction*> tx_logs;

    public:
        // method for Txgenerator
        void bootstrap(const char *statefile, const char *keyfile);
        std::string generate();
        void sendTx(int data_fd, std::string hexTx);

        //method for Monitoring node
        struct BlockInfo MakeBlockInfo(uint256 _blockhash, uint256 _prevblockhash, uint32_t _timestamp, unsigned long _txcount);
        void LoadBlock(int data_fd);
        bool isMonitoring = false;
    };
}


#endif //BLEEP_BITCOIN_NODE_PRIMITIVES_H
