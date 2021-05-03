//
// Created by ilios on 21. 2. 15..
//
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

using namespace tpstest;
using namespace std;

void BitcoinNodePrimitives::OpAfterConnect(int conn_fd) {
    cout << "OpAfterConnect for node [" << GetIP() << "]" << "\n";
    switch (_type) {
        case NodeType::TxGenerator:
        case NodeType::MonitoringNode: {
          // send initializing version message
          const unsigned char MessageStartChars[4] = {0xf9, 0xbe, 0xb4, 0xd9}; // for mainnet f9beb4d9

          // their_addr
          assert(_targetPort != -1 && _targetIP != "");
          CAddress their_addr;
          struct sockaddr_in new_addr;    /* my address information */
          new_addr.sin_family = AF_INET;         /* host byte order */
          new_addr.sin_port = htons(_targetPort);     /* short, network byte order */
          new_addr.sin_addr.s_addr = inet_addr(_targetIP.c_str());
          bzero(&(new_addr.sin_zero), 8);        /* zero the rest of the struct */

          if (!their_addr.SetSockAddr((const struct sockaddr *) &new_addr)) {
            LogPrintf("Warning: Unknown socket family\n");
          }

          // version message
          ServiceFlags nLocalNodeServices = ServiceFlags(NODE_NETWORK | NODE_WITNESS | NODE_NETWORK_LIMITED);
          uint64_t nonce = 0;
          int myNodeStartingHeight = 0;
          CAddress addrMe = CAddress(CService(), nLocalNodeServices);

          CSerializedNetMsg msg = CNetMsgMaker(INIT_PROTO_VERSION).Make(NetMsgType::VERSION, PROTOCOL_VERSION,
                                                                        (uint64_t) nLocalNodeServices, GetTime(),
                                                                        their_addr, addrMe, nonce, strSubVersion,
                                                                        myNodeStartingHeight, true);

          size_t nMessageSize = msg.data.size();
          //size_t nTotalSize = nMessageSize + CMessageHeader::HEADER_SIZE;
          LogPrint(BCLog::NET, "sending %s (%d bytes) \n", SanitizeString(msg.command.c_str()), nMessageSize);

          vector<unsigned char> serializedHeader;
          serializedHeader.reserve(CMessageHeader::HEADER_SIZE);
          uint256 hash = Hash(msg.data.data(), msg.data.data() + nMessageSize);
          CMessageHeader hdr(MessageStartChars, msg.command.c_str(), nMessageSize);
          memcpy(hdr.pchChecksum, hash.begin(), CMessageHeader::CHECKSUM_SIZE);

          CVectorWriter{SER_NETWORK, INIT_PROTO_VERSION, serializedHeader, 0, hdr};

          SendMsg(conn_fd, serializedHeader);
          if (nMessageSize) {
            SendMsg(conn_fd, msg.data);
          }

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
    // recv to RecvBuffer
    TCPControl &tcpControl = GetTCPControl(data_fd);
    tcpControl.AppendToRecvBuffer(recv_str);

    // get all received data stream
    string &recvbufstr = tcpControl.GetRecvBuffer();


    while (recvbufstr.size() > 0) {
        // first, dump a message header
        CNetMessage msg(Params().MessageStart(), SER_NETWORK,
                        INIT_PROTO_VERSION); // error when bitcoin is not initialized
        int headerReadSize = msg.readHeader(recvbufstr.c_str(), recvbufstr.size());
        if (headerReadSize < 0) {// error while reading header
            std::cout << "error while reading header" << "\n";
            assert(-1);
        }
        if (!msg.in_data) // header is not fully received
            return;

        // second, dump a message
        string msgstr = recvbufstr.substr(headerReadSize);
        int msgReadSize = msg.readData(msgstr.c_str(),
                                       msgstr.size());  // TODO : can be optimized to check size before dumping
        if (msgReadSize < 0) {// error??
            std::cout << "unrecognized error while reading data" << "\n";
            assert(-1);
        }
        if (msg.in_data && msg.hdr.nMessageSize > MAX_PROTOCOL_MESSAGE_LENGTH) {
            cout << "Oversized message" << "\n";
            exit(-1);
        }

        // third, parse a message
        if (!msg.complete()) {
            // data is not fully received
            return;
        } else {
            // data is fully received, so handle the message
            const unsigned char MessageStartChars[4] = {0xf9, 0xbe, 0xb4, 0xd9}; // for mainnet f9beb4d9

            if (memcmp(msg.hdr.pchMessageStart, MessageStartChars, CMessageHeader::MESSAGE_START_SIZE) != 0) {
                cout << "INVALID MESSAGESTART " << msg.hdr.GetCommand() << "\n";
                exit(-1);
            }


            // Read header
            CMessageHeader &hdr = msg.hdr;
            if (!hdr.IsValid(MessageStartChars)) {
                LogPrint(BCLog::NET, "PROCESSMESSAGE: ERRORS IN HEADER %s\n", hdr.GetCommand());
                exit(-1);
            }
            string strCommand = hdr.GetCommand();
            CDataStream &vRecv = msg.vRecv;

//            std::cout<<"OpafterRecv : <<" <<strCommand <<" "<<data_fd<<"\n";

            if (strCommand == NetMsgType::VERSION) {
                int64_t nTime;
                CAddress addrMe;
                CAddress addrFrom;
                uint64_t nNonce = 1;
                uint64_t nServiceInt;

                int nVersion;
                string cleanSubVer;
                int nStartingHeight = -1;
                bool fRelay = true;

                vRecv >> nVersion >> nServiceInt >> nTime >> addrMe;

                if (!vRecv.empty())
                    vRecv >> addrFrom >> nNonce;
                if (!vRecv.empty()) {
                    string strSubVer;
                    vRecv >> LIMITED_STRING(strSubVer, MAX_SUBVERSION_LENGTH);
                    cleanSubVer = SanitizeString(strSubVer);
                }
                if (!vRecv.empty()) {
                    vRecv >> nStartingHeight;
                }
                if (!vRecv.empty())
                    vRecv >> fRelay;

                {
                    // send verack message
                    CSerializedNetMsg verack_msg = CNetMsgMaker(INIT_PROTO_VERSION).Make(NetMsgType::VERACK);
                    size_t nMessageSize = verack_msg.data.size();
                    LogPrint(BCLog::NET, "sending %s (%d bytes) \n", SanitizeString(verack_msg.command.c_str()),
                             nMessageSize);

                    vector<unsigned char> verack_serializedHeader;
                    verack_serializedHeader.reserve(CMessageHeader::HEADER_SIZE);
                    uint256 hash = Hash(verack_msg.data.data(), verack_msg.data.data() + nMessageSize);
                    CMessageHeader verack_hdr(MessageStartChars, verack_msg.command.c_str(), nMessageSize);
                    memcpy(verack_hdr.pchChecksum, hash.begin(), CMessageHeader::CHECKSUM_SIZE);

                    CVectorWriter{SER_NETWORK, INIT_PROTO_VERSION, verack_serializedHeader, 0, verack_hdr};

                    SendMsg(data_fd, verack_serializedHeader);
                    if (nMessageSize)
                        SendMsg(data_fd, verack_msg.data);
                }
            } else if (strCommand == NetMsgType::PING) {
                uint64_t nonce = 0;
                vRecv >> nonce;

                CSerializedNetMsg replymsg = CNetMsgMaker(PROTOCOL_VERSION).Make(NetMsgType::PONG, nonce);

                size_t nMessageSize = replymsg.data.size();
                //size_t nTotalSize = nMessageSize + CMessageHeader::HEADER_SIZE;
                // LogPrint(BCLog::NET, "sending %s (%d bytes) \n", SanitizeString(msg.command.c_str()), nMessageSize);

                vector<unsigned char> serializedHeader;
                serializedHeader.reserve(CMessageHeader::HEADER_SIZE);
                uint256 hash = Hash(replymsg.data.data(), replymsg.data.data() + nMessageSize);
                CMessageHeader replymsghdr(MessageStartChars, replymsg.command.c_str(), nMessageSize);
                memcpy(replymsghdr.pchChecksum, hash.begin(), CMessageHeader::CHECKSUM_SIZE);

                CVectorWriter{SER_NETWORK, INIT_PROTO_VERSION, serializedHeader, 0, replymsghdr};

                SendMsg(data_fd, serializedHeader);
                if (nMessageSize)
                    SendMsg(data_fd, replymsg.data);

            } else if (strCommand == NetMsgType::INV){

                std::vector<CInv> vInv;
                vRecv >> vInv;
                for (CInv &inv : vInv)
                {

                    if (inv.type == MSG_TX) {
                        std::cout<<"[INV] MSGTX: hash = "<<inv.hash.ToString()<<" from = "<<data_fd<<"\n";
                        RegisterTx(inv.hash.ToString());

                    } else if (inv.type == MSG_BLOCK) {
                        std::string block_hash = inv.hash.ToString();

                         //send block message
                        CSerializedNetMsg replymsg = CNetMsgMaker(PROTOCOL_VERSION).Make(NetMsgType::GETDATA, vInv);

                        size_t nMessageSize = replymsg.data.size();

                        vector<unsigned char> serializedHeader;
                        serializedHeader.reserve(CMessageHeader::HEADER_SIZE);
                        uint256 hash = Hash(replymsg.data.data(), replymsg.data.data() + nMessageSize);
                        CMessageHeader replymsghdr(MessageStartChars, replymsg.command.c_str(), nMessageSize);
                        memcpy(replymsghdr.pchChecksum, hash.begin(), CMessageHeader::CHECKSUM_SIZE);

                        CVectorWriter{SER_NETWORK, INIT_PROTO_VERSION, serializedHeader, 0, replymsghdr};

                        SendMsg(data_fd, serializedHeader);
                        if (nMessageSize)
                            SendMsg(data_fd, replymsg.data);
                    } else {
                        std::string invhash = inv.hash.ToString();
                            cout<<"inv msg else "<<invhash<<"\n";
                    }
                }

            }else if(strCommand == NetMsgType::VERACK) {
                BitcoinNodePrimitives::LoadBlock(data_fd);

            } else if (strCommand == NetMsgType::BLOCK) {
                if (_temp_isMointor()) {
                    std::shared_ptr<CBlock> pblock = std::make_shared<CBlock>();
                    vRecv >> *pblock;

                    std::cout<<"[INV] MSGBLOCK: hash = "<<pblock->GetHash().ToString()<<" txcnt = "<<pblock->vtx.size()<<" from = "<<data_fd <<"\n";

                    // create block structure
                    block* bp = new block(pblock->GetHash().GetHex(), pblock->hashPrevBlock.GetHex(), pblock->nTime);
                    unsigned long int netTxLatency = 0;
                    for (int i = 0; i<pblock->vtx.size(); i++) {
                        bp->pushTxHash(pblock->vtx[i]->GetHash().GetHex());
                    }

                    // add block to forest
                    if (!bf.add_block(bp)) {
                        delete bp;  // if already exists, free allocated memory
                    }

                    if (pblock->vtx.size() >= 2) {
                        for (int i = 1; i<pblock->vtx.size(); i++) {
                            assert(pblock->nTime >= global_txtimepool->get_txtime(pblock->vtx[i]->GetHash().GetHex()));
                            netTxLatency += pblock->nTime - global_txtimepool->get_txtime(pblock->vtx[i]->GetHash().GetHex());
                        }
                        bp->setNetTxLatency(netTxLatency);
                    }

                    // get besttip, calculate tps
                    static block* best = nullptr;
                    bp = bf.get_besttip();
#define CONFIRMATION_COUNT  4
                    if (bp && bp->getParent() && (!best || best != bp)) {
                        best = bp;
                        uint32_t besttime = bp->getTime();
                        size_t txcount = 0;
                        int length_from_tip = 0;
                        size_t netConfirmedTxCount = 0;
                        unsigned long int netConfirmedTxLatency = 0;
                        while(bp->getParent()) {
                            txcount += bp->getTxCount();
                            if (length_from_tip > CONFIRMATION_COUNT) {
                                netConfirmedTxCount += bp->getTxCount() - 1;    // ignore coinbase tx
                                netConfirmedTxLatency += bp->getNetTxLatency();
                            }
                            bp = bp->getParent();
                            length_from_tip++;
                        }
                        uint32_t timebase = bp->getTime();
                        std::cout << "TPS = " << (txcount / ((double)besttime - timebase)) << "\n";
                        if (netConfirmedTxCount && netConfirmedTxLatency) {
                            std::cout << "Latency = " << (netConfirmedTxLatency / netConfirmedTxCount) << "seconds\n";
                        }
                    }
                }
            }

            // Maybe, recvBuffer can be updated more efficiently. (minimizing a duplication)
            std::string remain = recvbufstr.substr(headerReadSize + msgReadSize);
            tcpControl.SetRecvBuffer(remain);
        }

        // repeating the process until there is remaining data stream in RecvBuffer
        recvbufstr = tcpControl.GetRecvBuffer();
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
    int dividing_factor = 2;

    // step 1, 12
    CMutableTransaction txNew;  // version and locktime is automatically set

    auto source = unspent_keyvalues.front();
    CKey sourceKey = source.sourceKey;
    CTransaction* sourceTx = source.sourceTx;
    uint32_t sourceIn = source.nIn;
    CAmount fee = 1000;
    CAmount voutValue = (sourceTx->vout[sourceIn].nValue - fee) / dividing_factor;

    // step 2, 3, 4, 5, 6, 7
    CTxIn txin_proto(COutPoint(sourceTx->GetHash(), sourceIn), sourceTx->vout[sourceIn].scriptPubKey, 0xffffffff);
    txNew.vin.push_back(txin_proto);

    // txout build
    std::vector<CKey> predata;
    for(int i=0; i<dividing_factor; i++) {
        CKey dest = _generateKey();
        CTxDestination receiveDest = GetDestinationForKey(dest.GetPubKey(), OutputType::LEGACY);
        CScript scriptReceive = GetScriptForDestination(receiveDest);
        CTxOut receive_txout(voutValue, scriptReceive);
        txNew.vout.push_back(receive_txout);
        predata.push_back(dest);
    }

    // get hash
    CHashWriter txhasher(SER_GETHASH, 0);
    txhasher << txNew << (uint32_t)SIGHASH_ALL;
    uint256 hash = txhasher.GetHash();
    // sign
    std::vector<unsigned char> vchSig;
    sourceKey.Sign(hash, vchSig);
    // add hashtype
    vchSig.push_back((unsigned char)SIGHASH_ALL);
    txNew.vin.clear();
    CTxIn txin_proto2(COutPoint(sourceTx->GetHash(), sourceIn), CScript() << vchSig << ToByteVector(sourceKey.GetPubKey()), 0xffffffff);
    txNew.vin.push_back(txin_proto2);

    tx_logs.push(sourceTx);
    unspent_keyvalues.pop();
    CTransaction* tx = new CTransaction(txNew);
    for(uint32_t i=0; i<dividing_factor; i++) {
        unspent_keyvalues.push({predata[i], tx, i});
    }
    std::cout<<"Debug - created tx's hash:"<<tx->GetHash().GetHex()<<"\n";

    return EncodeHexTx(*tx);
}

void BitcoinNodePrimitives::sendTx(int data_fd, std::string hexTx) {
    CMutableTransaction mtx;
    if (!DecodeHexTx(mtx, hexTx, true))
        throw std::runtime_error("invalid transaction encoding");
    CTransaction tx(mtx);
    const unsigned char MessageStartChars[4] = {0xf9, 0xbe, 0xb4, 0xd9}; // for mainnet f9beb4d9
    CSerializedNetMsg msg = CNetMsgMaker(PROTOCOL_VERSION).Make(SERIALIZE_TRANSACTION_NO_WITNESS, NetMsgType::TX, tx);
    size_t nMessageSize = msg.data.size();
    std::vector<unsigned char> serializedHeader;
    serializedHeader.reserve(CMessageHeader::HEADER_SIZE);
    uint256 hash = Hash(msg.data.data(), msg.data.data() + nMessageSize);
    CMessageHeader hdr(MessageStartChars, msg.command.c_str(), nMessageSize);
    memcpy(hdr.pchChecksum, hash.begin(), CMessageHeader::CHECKSUM_SIZE);

    CVectorWriter{SER_NETWORK, PROTOCOL_VERSION, serializedHeader, 0, hdr};
    global_txtimepool->register_txtime(tx.GetHash().GetHex(), GetAdjustedTime());
    SendMsg(data_fd, serializedHeader);
    if (nMessageSize) {
        SendMsg(data_fd, msg.data);
    }

}

void BitcoinNodePrimitives::LoadBlock(int data_fd) {
    //load block message in coinflip_hash.txt
    std::string path = "./data/coinflip_hash.txt";

    const unsigned char MessageStartChars2[4] = {0xf9, 0xbe, 0xb4, 0xd9}; // for mainnet f9beb4d9
    ifstream file (path);
    std::string line;
    if (file.is_open())
    {
        while ( getline (file,line) )
        {

            std::vector<CInv> vInv2(1);
            uint256 block_hash2 = uint256S(line);
            vInv2[0] = CInv(2, block_hash2);

            //send block message
            CSerializedNetMsg replymsg2 = CNetMsgMaker(PROTOCOL_VERSION).Make(SERIALIZE_TRANSACTION_NO_WITNESS,NetMsgType::GETDATA, vInv2);

            size_t nMessageSize2 = replymsg2.data.size();
//            std::cout<<"sending %s (%d bytes) "<<block_hash2.ToString()<<" \n";


            vector<unsigned char> serializedHeader2;
            serializedHeader2.reserve(CMessageHeader::HEADER_SIZE);
            uint256 hash2 = Hash(replymsg2.data.data(), replymsg2.data.data() + nMessageSize2);
            CMessageHeader replymsghdr2(MessageStartChars2, replymsg2.command.c_str(), nMessageSize2);
            memcpy(replymsghdr2.pchChecksum, hash2.begin(), CMessageHeader::CHECKSUM_SIZE);

            CVectorWriter{SER_NETWORK, INIT_PROTO_VERSION, serializedHeader2, 0, replymsghdr2};

            SendMsg(data_fd, serializedHeader2);
            if (nMessageSize2)
                SendMsg(data_fd, replymsg2.data);

        }
        file.close();
    }

}
