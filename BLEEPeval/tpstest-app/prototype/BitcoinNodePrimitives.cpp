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

            std::cout<<"OpafterRecv : <<" <<strCommand <<"\n";

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

                if (_type == NodeType::TxGenerator) {
                    // VERSION message from inbound connection
                    // send version message (reply)
                    ServiceFlags nLocalNodeServices = ServiceFlags(NODE_NETWORK | NODE_WITNESS | NODE_NETWORK_LIMITED);
                    uint64_t nonce = 0;
                    int myNodeStartingHeight = nStartingHeight;


                    cout << "version is from " << addrFrom.ToString() << ", to " << addrMe.ToString() << "\n";
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
                    cout << "their_addr:" << their_addr.ToString() << "\n";


                    CAddress addrMeForVersionReply = CAddress(CService(), nLocalNodeServices);


                    CSerializedNetMsg replymsg = CNetMsgMaker(INIT_PROTO_VERSION).Make(NetMsgType::VERSION,
                                                                                       PROTOCOL_VERSION,
                                                                                       (uint64_t) nLocalNodeServices,
                                                                                       nTime,
                                                                                       their_addr,
                                                                                       addrMeForVersionReply, nonce,
                                                                                       strSubVersion,
                                                                                       myNodeStartingHeight, true);

                    size_t nMessageSize = replymsg.data.size();
                    //size_t nTotalSize = nMessageSize + CMessageHeader::HEADER_SIZE;
                    LogPrint(BCLog::NET, "sending %s (%d bytes) \n", SanitizeString(replymsg.command.c_str()),
                             nMessageSize);

                    vector<unsigned char> serializedHeader;
                    serializedHeader.reserve(CMessageHeader::HEADER_SIZE);
                    uint256 hash = Hash(replymsg.data.data(), replymsg.data.data() + nMessageSize);
                    CMessageHeader replymsghdr(MessageStartChars, replymsg.command.c_str(), nMessageSize);
                    memcpy(replymsghdr.pchChecksum, hash.begin(), CMessageHeader::CHECKSUM_SIZE);

                    CVectorWriter{SER_NETWORK, INIT_PROTO_VERSION, serializedHeader, 0, replymsghdr};

                    SendMsg(data_fd, serializedHeader);
                    if (nMessageSize)
                        SendMsg(data_fd, replymsg.data);
                }

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
                cout << "received PING" << "\n";
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
    sourceTx = new CTransaction(mtx);


//    CDataStream stream(ParseHex(txstr), SER_NETWORK, CLIENT_VERSION);
//    sourceTx = new CTransaction(deserialize, stream);

    std::ifstream key_ifs(keyfile);
    std::string keystr((std::istreambuf_iterator<char>(key_ifs)), (std::istreambuf_iterator<char>()));
    secret = DecodeSecret(keystr);
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
    // CKey::Sign(const uint256 &hash, std::vector<unsigned char>& vchSig, bool grind, uint32_t test_case)
    // -> hash, vchSig만 있어도 되긴 함.
    // step 1, 12
    CMutableTransaction txNew;  // version and locktime is automatically set

    // step 2, 3, 4, 5, 6, 7
    CTxIn txin_proto(COutPoint(sourceTx->GetHash(), 0), sourceTx->vout[0].scriptPubKey, 0xffffffff);
    txNew.vin.push_back(txin_proto);

    // txout build
    CTxDestination receiveDest = GetDestinationForKey(secret.GetPubKey(), OutputType::LEGACY);
    CScript scriptReceive = GetScriptForDestination(receiveDest);
    CTxOut receive_prototype_txout(0, scriptReceive);
    size_t nSize = GetSerializeSize(receive_prototype_txout);
//    CTxOut receive_txout(CFeeRate(DUST_RELAY_TX_FEE).GetFee(nSize) + 1, scriptReceive);
    CTxOut receive_txout(547, scriptReceive);   // debug
    txNew.vout.push_back(receive_txout);

    // dummy txchange build
    CKey changer = _generateKey();
    CTxDestination changeDest = GetDestinationForKey(changer.GetPubKey(), OutputType::LEGACY);
    CScript scriptChange = GetScriptForDestination(changeDest);
    CTxOut change_prototype_txout(0, scriptChange);
    txNew.vout.push_back(change_prototype_txout);

    // get hash
    CHashWriter txhasher(SER_GETHASH, 0);
    txhasher << txNew << (uint32_t)SIGHASH_ALL;
    uint256 hash = txhasher.GetHash();
    // sign
    std::vector<unsigned char> vchSig;
    secret.Sign(hash, vchSig);
    // add hashtype
    vchSig.push_back((unsigned char)SIGHASH_ALL);
    txNew.vin.clear();
    CTxIn txin_proto2(COutPoint(sourceTx->GetHash(), 0), CScript() << vchSig << ToByteVector(secret.GetPubKey()), 0xffffffff);
    txNew.vin.push_back(txin_proto2);

    // recalculate changes
    CTransaction tx(txNew);
    CAmount nBytes = tx.GetTotalSize();
//    CAmount nFeeNeeded = CFeeRate(DEFAULT_FALLBACK_FEE).GetFee(nBytes);
//    CAmount change = sourceTx->vout[0].nValue - nFeeNeeded - 1;
    CAmount nFeeNeeded = CFeeRate(DEFAULT_FALLBACK_FEE).GetFee(nBytes) + 547;   // debug
    CAmount change = sourceTx->vout[0].nValue - nFeeNeeded - 1000;                 // debug
    CTxOut change_txout(change, scriptChange);
    txNew.vout.pop_back();
    txNew.vout.insert(txNew.vout.begin(), change_txout);

    // do signing again
    txNew.vin.clear();
    CTxIn txin_proto3(COutPoint(sourceTx->GetHash(), 0), sourceTx->vout[0].scriptPubKey, 0xffffffff);
    txNew.vin.push_back(txin_proto3);
    // get hash
    CHashWriter txhasher2(SER_GETHASH, 0);
    txhasher2 << txNew << (uint32_t)SIGHASH_ALL;
    uint256 hash2 = txhasher2.GetHash();
    // sign
    std::vector<unsigned char> vchSig2;
    secret.Sign(hash2, vchSig2);
    // add hashtype
    vchSig2.push_back((unsigned char)SIGHASH_ALL);

    txNew.vin.clear();
    CTxIn txin_proto4(COutPoint(sourceTx->GetHash(), 0), CScript() << vchSig2 << ToByteVector(secret.GetPubKey()), 0xffffffff);
    txNew.vin.push_back(txin_proto4);

    // swap states
    CPubKey pubkey = secret.GetPubKey();
    uint160 pubkeyHash;
    CHash160().Write(pubkey.begin(), pubkey.size()).Finalize(pubkeyHash.begin());
    std::cout<<"Debug - pub:"<<pubkeyHash.GetHex()<<"\n";
    secret = changer;
    CTransaction* prev = sourceTx;
    sourceTx = new CTransaction(txNew);
    delete prev;
    std::cout<<"Debug - in script:"<<FormatScript(sourceTx->vin[0].scriptSig)<<"\n";
    std::cout<<"Debug - out script:"<<FormatScript(sourceTx->vout[0].scriptPubKey)<<"\n";
    std::cout<<"Debug - out script:"<<FormatScript(sourceTx->vout[1].scriptPubKey)<<"\n\n";

    return EncodeHexTx(*sourceTx);
}

void BitcoinNodePrimitives::sendTx(int data_fd, std::string hexTx) {
    CMutableTransaction mtx;
    if (!DecodeHexTx(mtx, hexTx, true))
        throw std::runtime_error("invalid transaction encoding");
    std::cout<<"Debug - send tx:"<<hexTx<<"\n";
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
    SendMsg(data_fd, serializedHeader);
    if (nMessageSize) {
        SendMsg(data_fd, msg.data);
    }

}