// "Copyright [2021] <kaistshadow>"

//
// Created by ilios on 21. 2. 15..
//

#include <string>
#include <vector>
#include <list>
#include <iostream>
#include "BitcoinNodePrimitives.h"
#include <random>
#include <algorithm>

// bitcoin-specific data management
#include <net.h>
#include <netmessagemaker.h>
#include <util/strencodings.h>
#include <chainparams.h>

// bitcoin-specific tx generation
#include <config/bitcoin-config.h>
#include <clientversion.h>
#include <key_io.h>
#include <policy/policy.h>
#include <wallet/wallet.h>
#include <core_io.h>
#include <outputtype.h>
#include "BL2_peer_connectivity/Message.h"
#include "BL2_peer_connectivity/Peer.h"
#include "../../../shadow/src/main/core/work/shd-message.h"

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <utility/blocks.cpp>
#include <boost/archive/binary_iarchive.hpp>


using namespace tpstest;
using namespace std;

void BitcoinNodePrimitives::OpAfterConnect(int conn_fd) {
    cout << "OpAfterConnect for node [" << GetIP() << "]" << "\n";
    switch (_type) {
        case NodeType::TxGenerator:
        case NodeType::MonitoringNode: {
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
    }
}

void BitcoinNodePrimitives::OpAfterConnected(int data_fd) {
    // do nothing
}


BitcoinNodePrimitives::BlockInfo BitcoinNodePrimitives::MakeBlockInfo(uint256 _blockhash, uint256 _prevblockhash, uint32_t _timestamp, unsigned long _txcount) {
    struct BitcoinNodePrimitives::BlockInfo newBlock;
    newBlock.prevblockhash = _prevblockhash.ToString();
    newBlock.blockhash = _blockhash.ToString();
    newBlock.txcount = _txcount;
    newBlock.timestamp = _timestamp;
    return newBlock;
}


void BitcoinNodePrimitives::OpAfterRecv(int data_fd, string recv_str) {
    switch (_type) {
        case NodeType::MonitoringNode:
        case NodeType::TxGenerator: {
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
                    } else {
                        break;
                    }

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
                            std::cout << "send Pong message to " << data_fd << " \n";
                        } else if (msg->GetType() == "POWBLOCK-INV") {
                            // 1. msg hash 값 확인
                            std::shared_ptr<libBLEEP_BL::POWBlockGossipInventory> inv = std::static_pointer_cast<libBLEEP_BL::POWBlockGossipInventory>(msg->GetObject());
                            auto hashes = inv->GetHashlist();
                            for (auto hash : hashes) {
                                // 2. getdata 메세지 보내기
                                std::shared_ptr<libBLEEP_BL::MessageObject> ptrToObj = std::make_shared<libBLEEP_BL::POWBlockGossipGetData>(hash);
                                std::shared_ptr<libBLEEP_BL::Message> Msg = std::make_shared<libBLEEP_BL::Message>(
                                libBLEEP_BL::PeerId(GetIP()), libBLEEP_BL::PeerId(""), "POWBLOCK-GETDATA", ptrToObj);

                                // serialize message obj into an std::string
                                std::string serial_str;
                                boost::iostreams::back_insert_device<std::string> inserter(serial_str);
                                boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > outs(inserter);
                                boost::archive::binary_oarchive oa(outs);
                                oa << Msg;
                                outs.flush();

                                SendMsg(data_fd, BLEEP_MAGIC);
                                SendMsg(data_fd, serial_str.size());
                                SendMsg(data_fd, serial_str);
                            }
                        } else if (msg->GetType() == "POWBLOCK-BLK") {
                            std::shared_ptr<libBLEEP_BL::POWBlockGossipBlk> getdata = std::static_pointer_cast<libBLEEP_BL::POWBlockGossipBlk>(msg->GetObject());
                            std::shared_ptr<libBLEEP_BL::POWBlock> blkptr = getdata->GetBlock();
                            std::cout << "powblock-blk message " << blkptr->GetBlockHash() << " / prevhash = " << blkptr->GetPrevBlockHash() << " / tx cont = " << blkptr->GetTransactions().size() << "\n";

                            // block insert to blockforest
                            std::vector<std::string> txlist;
                            std::list<std::shared_ptr<libBLEEP_BL::SimpleTransaction>> block_txs = blkptr->GetTransactions();
                            for (auto transaction : block_txs) {
                                txlist.push_back(transaction->_id.GetTxHash().str());
                            }

                            if (blkptr->GetPrevBlockHash() == libBLEEP::UINT256_t(NULL)) {
                                isMonitoring = true;
                            }
                            if (isMonitoring) {
                                uint32_t time = uint32_t(blkptr->GetTimestamp());
                                if (!UpdateBlock(blkptr->GetBlockHash().str(), blkptr->GetPrevBlockHash().str(), time, txlist)) {
                                    std::cout << "block is already exist / blockhash = " << blkptr->GetBlockHash().str() << "\n";
                                }
                                std::cout << "block successfully register / blockhash = " << blkptr->GetBlockHash().str() << "\n";
                            }
                        } else if (msg->GetType() =="TXGOSSIP-INV") {
                            std::shared_ptr<libBLEEP_BL::TxGossipInventory> inv = std::static_pointer_cast<libBLEEP_BL::TxGossipInventory>(msg->GetObject());
                            auto tids = inv->GetTransactionIds();
                            std::vector<libBLEEP_BL::SimpleTransactionId> getdataIds;
                            for (auto tid : tids) {
                                global_txtimepool->register_txtime(tid.GetTxHash().str(), libBLEEP::GetGlobalClock());
                            }
                        }

                        // Maybe, recvBuffer can be updated efficiently. (minimizing a duplication)
                        std::string remain = recvbufstr.substr(BLEEP_MAGIC_SIZE + sizeof(int) + msg_size);
                        tcpControl.SetRecvBuffer(remain);
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }
            break;
        }
    }
}

void BitcoinNodePrimitives::OpAfterDisconnect() {
    // do nothing
}

void BitcoinNodePrimitives::bootstrap(const char* statefile, const char* keyfile) {
    std::ifstream state_ifs(statefile);
    std::string txstr((std::istreambuf_iterator<char>(state_ifs)), (std::istreambuf_iterator<char>()));
    CMutableTransaction mtx;
    DecodeHexTx(mtx, txstr, true);
    CTransaction* tx = new CTransaction(mtx);

    std::ifstream key_ifs(keyfile);
    std::string keystr((std::istreambuf_iterator<char>(key_ifs)), (std::istreambuf_iterator<char>()));
    CKey key = DecodeSecret(keystr);

    unspent_keyvalues.push({key, tx, 0});
}
#define COMPRESSED_KEY
CKey _generateKey() {
    CKey secret;
#ifdef COMPRESSED_KEY
    secret.MakeNewKey(true);
#else
    secret.MakeNewKey(false);
#endif
    CPubKey pubkey = secret.GetPubKey();
    assert(secret.VerifyPubKey(pubkey));

    return secret;
}
// from state and key, make next state and key, return serialized tx with size
std::string BitcoinNodePrimitives::generate() {
// generate random transaction
    srand((unsigned int)time(0));
    int sender_id = rand() % 100;
    int receiver_id = rand() % 100;
    float amount = (float) (rand() % 100000);

    std::shared_ptr<libBLEEP_BL::SimpleTransaction> tx = std::make_shared<libBLEEP_BL::SimpleTransaction>(sender_id, receiver_id, amount);

    unsigned char hash_out[32];
    std::string sender_id_str = std::to_string(sender_id);
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (const unsigned char*)std::to_string(sender_id).c_str(), sizeof(int));
    sha256_update(&ctx, (const unsigned char*)std::to_string(receiver_id).c_str(), sizeof(int));
    sha256_update(&ctx, (const unsigned char*)std::to_string(amount).c_str(), sizeof(float));

    double timestamp = libBLEEP::GetGlobalClock();
    sha256_update(&ctx, (const unsigned char*)&timestamp, sizeof(double));
    sha256_final(&ctx, hash_out);
    libBLEEP::UINT256_t hash_out_256(hash_out, 32);
    tx->_id.SetTxHash(hash_out_256);


    std::vector<std::shared_ptr<libBLEEP_BL::SimpleTransaction>> txs;
    txs.push_back(tx);

    std::shared_ptr<libBLEEP_BL::MessageObject> ptrToObj = std::make_shared<libBLEEP_BL::TxGossipTxs>(txs);
    _txToBroadcast = ptrToObj;
    global_txtimepool->register_txtime(hash_out_256.str(), libBLEEP::GetGlobalClock());
    std::cout << "txgentimer generate new tx = " << tx->_id.GetTxHash() << "\n";

    return tx->_id.GetTxHash().str();
}

void BitcoinNodePrimitives::sendTx(int data_fd, std::string hexTx) {
    std::shared_ptr<libBLEEP_BL::MessageObject> ptrToObj = _txToBroadcast;
    std::shared_ptr<libBLEEP_BL::Message> message = std::make_shared<libBLEEP_BL::Message>(
            libBLEEP_BL::PeerId(GetIP()), libBLEEP_BL::PeerId(""), "TXGOSSIP-TXS", ptrToObj);

    // serialize message obj into an std::string
    std::string serial_str;
    boost::iostreams::back_insert_device<std::string> inserter(serial_str);
    boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
    boost::archive::binary_oarchive oa(s);
    oa << message;
    s.flush();
    SendMsg(data_fd, BLEEP_MAGIC);
    SendMsg(data_fd, serial_str.size());
    SendMsg(data_fd, serial_str);
    std::cout << "send tx message to " << data_fd << "\n";
}

// void BitcoinNodePrimitives::LoadBlock(int data_fd) {
// //    if(!UpdateBlock(blkptr->GetBlockHash().str(), blkptr->GetPrevBlockHash().str(),time, txlist)){
// }
