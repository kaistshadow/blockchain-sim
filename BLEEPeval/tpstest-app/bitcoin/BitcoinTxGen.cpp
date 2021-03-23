//
// Created by csrc on 21. 3. 16..
//

#include "BitcoinTxGen.h"

#include <config/bitcoin-config.h>

#include <clientversion.h>
#include <serialize.h>
#include <util/strencodings.h>
#include <streams.h>
#include <fstream>
#include <key_io.h>
#include <script/standard.h>
#include <wallet/wallet.h>
#include <core_io.h>
#include <policy/policy.h>
#include <script/interpreter.h>

BitcoinTxgen::BitcoinTxgen(const char *statefile, const char *keyfile) {
    std::ifstream state_ifs(statefile);
    std::string txstr((std::istreambuf_iterator<char>(state_ifs)), (std::istreambuf_iterator<char>()));
    CDataStream stream(ParseHex(txstr), SER_DISK, CLIENT_VERSION);
    sourceTx = new CTransaction(deserialize, stream);

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
std::string BitcoinTxgen::generate() {
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
    CTxOut receive_txout(CFeeRate(DUST_RELAY_TX_FEE).GetFee(nSize) + 1, scriptReceive);
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
    CDataStream scriptStream(SER_DISK, PROTOCOL_VERSION);
    scriptStream << vchSig;   // debug
    CDataStream pubkeyStream(SER_DISK, PROTOCOL_VERSION);
    pubkeyStream << secret.GetPubKey();
    unsigned char pubkeyLen = (HexStr(pubkeyStream.begin(), pubkeyStream.end()).length() / 2);
    scriptStream << pubkeyLen << secret.GetPubKey();
    txNew.vin.clear();
    CTxIn txin_proto2(COutPoint(sourceTx->GetHash(), 0), CScript() << ParseHex(HexStr(scriptStream.begin(), scriptStream.end())), 0xffffffff);
    txNew.vin.push_back(txin_proto2);

    // recalculate changes
    CTransaction tx(txNew);
    CAmount nBytes = tx.GetTotalSize();
    CAmount nFeeNeeded = CFeeRate(DEFAULT_FALLBACK_FEE).GetFee(nBytes);
    CAmount change = sourceTx->vout[0].nValue - nFeeNeeded - 1;
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
    CDataStream scriptStream2(SER_DISK, PROTOCOL_VERSION);
    scriptStream2 << vchSig2;   // debug
    CDataStream pubkeyStream2(SER_DISK, PROTOCOL_VERSION);
    pubkeyStream2 << secret.GetPubKey();
    unsigned char pubkeyLen2 = (HexStr(pubkeyStream2.begin(), pubkeyStream2.end()).length() / 2);
    scriptStream2 << pubkeyLen2 << secret.GetPubKey();
    txNew.vin.clear();
    CTxIn txin_proto4(COutPoint(sourceTx->GetHash(), 0), CScript() << ParseHex(HexStr(scriptStream2.begin(), scriptStream2.end())), 0xffffffff);
    txNew.vin.push_back(txin_proto4);

    // swap states
    secret = changer;
    CTransaction* prev = sourceTx;
    sourceTx = new CTransaction(txNew);
    delete prev;

    return EncodeHexTx(*sourceTx);
}