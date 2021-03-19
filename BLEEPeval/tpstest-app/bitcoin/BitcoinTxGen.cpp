//
// Created by csrc on 21. 3. 16..
//

#include "BitcoinTxGen.h"

#include <key_io.h>
#include <validation.h>
#include <consensus/validation.h>
#include <wallet/coincontrol.h>
#include <rpc/util.h>
#include <util/moneystr.h>
#include <util/validation.h>
#include <policy/policy.h>
#include <wallet/wallet.h>
#include <script/sigcache.h>

class BitcoinTxGen {
private:
    CWallet* pwallet;
    CWallet* receiverWallet;
public:
    BitcoinTxGen(std::string str);
    unsigned char* genereteTx();
};

BitcoinTxGen::BitcoinTxGen(std::string str) {
    SelectParams("main");
    const CChainParams& chainparams = Params();

    // reindexing history
    int nFile = 0;
    while (true) {
        FlatFilePos pos(nFile, 0);
        if (!fs::exists(GetBlockPosFilename(pos)))
            break; // No block files left to reindex
        FILE *file = OpenBlockFile(pos, true);
        if (!file)
            break; // This error is logged in OpenBlockFile
        LogPrintf("Reindexing block file blk%05u.dat...\n", (unsigned int)nFile);
        LoadExternalBlockFile(chainparams, file, &pos);
        nFile++;
    }

//    CKey k = DecodeSecret(str);
}

bool updateWallet(CTransactionRef tx, mapValue_t mapValue, std::vector<std::pair<std::string, std::string>> orderForm, CValidationState& state) {
    {
        auto locked_chain = chain().lock();
        LOCK(cs_wallet);

        CWalletTx wtxNew(this, std::move(tx));
        wtxNew.mapValue = std::move(mapValue);
        wtxNew.vOrderForm = std::move(orderForm);
        wtxNew.fTimeReceivedIsTxTime = true;
        wtxNew.fFromMe = true;

        WalletLogPrintf("CommitTransaction:\n%s", wtxNew.tx->ToString()); /* Continued */
        {

            // Add tx to wallet, because if it has change it's also ours,
            // otherwise just for transaction history.
            AddToWallet(wtxNew);

            // Notify that old coins are spent
            for (const CTxIn& txin : wtxNew.tx->vin)
            {
                CWalletTx &coin = mapWallet.at(txin.prevout.hash);
                coin.BindWallet(this);
                NotifyTransactionChanged(this, coin.GetHash(), CT_UPDATED);
            }
        }

        // Get the inserted-CWalletTx from mapWallet so that the
        // fInMempool flag is cached properly
        CWalletTx& wtx = mapWallet.at(wtxNew.GetHash());

        if (fBroadcastTransactions)
        {
            std::string err_string;
            if (!wtx.SubmitMemoryPoolAndRelay(err_string, true, *locked_chain)) {
                WalletLogPrintf("CommitTransaction(): Transaction cannot be broadcast immediately, %s\n", err_string);
                // TODO: if we expect the failure to be long term or permanent, instead delete wtx from the wallet and return failure.
            }
        }
    }
    return true;
}
unsigned char* BitcoinTxGen::generateTx() {
    CCoinControl coin_control;
    coin_control.m_avoid_address_reuse = true;
    // We also enable partial spend avoidance if reuse avoidance is set.
    coin_control.m_avoid_partial_spends |= coin_control.m_avoid_address_reuse;
    auto locked_chain = pwallet->chain().lock();
    CTxDestination address = DecodeDestination(Address);
    CAmount nValue = AmountFromValue("0.0001");
    mapValue_t mapValue;
    bool fSubtractFeeFromAmount = false;






    // SendMoney(interfaces::Chain::Lock& locked_chain, CWallet * const pwallet, const CTxDestination &address, CAmount nValue, bool fSubtractFeeFromAmount, const CCoinControl& coin_control, mapValue_t mapValue)
    CAmount curBalance = pwallet->GetBalance(0, coin_control.m_avoid_address_reuse).m_mine_trusted;
    // Check amount
    if (nValue <= 0)
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid amount");

    if (nValue > curBalance)
        throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, "Insufficient funds");

    // Parse Bitcoin address
    CScript scriptPubKey = GetScriptForDestination(address);

    // Create and send the transaction
    CAmount nFeeRequired;
    std::string strError;
    std::vector<CRecipient> vecSend;
    int nChangePosRet = -1;
    CRecipient recipient = {scriptPubKey, nValue, fSubtractFeeFromAmount};
    vecSend.push_back(recipient);
    CTransactionRef tx;
    if (!pwallet->CreateTransaction(*locked_chain, vecSend, tx, nFeeRequired, nChangePosRet, strError, coin_control)) {
        if (!fSubtractFeeFromAmount && nValue + nFeeRequired > curBalance)
            strError = strprintf("Error: This transaction requires a transaction fee of at least %s", FormatMoney(nFeeRequired));
        throw JSONRPCError(RPC_WALLET_ERROR, strError);
    }
    CValidationState state;
    if (!updateWallet(tx, std::move(mapValue), {} /* orderForm */, state)) {
        strError = strprintf("Error: The transaction was rejected! Reason given: %s", FormatStateMessage(state));
        throw JSONRPCError(RPC_WALLET_ERROR, strError);
    }
    return tx;
}

#define COMPRESSED_KEY
CKey generateKey() {
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

class TemporaryContainer : public SigningProvider {
private:
    CKey sender;
    CTransactionRef prevTx;

    CKey receiver;
public:
    TemporaryContainer(std::string encoded_CKey, uint256 prevTxHash, CAmount initial_coin);
    void generateTransaction(CTransactionRef& tx);

    bool GetPubKey(const CKeyID &address, CPubKey &pubkey) const override;
    bool GetCScript(const CScriptID &scriptid, CScript &script) const override;
};
bool TemporaryContainer::GetPubKey(const CKeyID &address, CPubKey &pubkey) const {
    if (sender.GetPubKey().GetID() == address) {
        return true;
    }
    return false;
}
TemporaryContainer::TemporaryContainer(std::string encoded_CKey, uint256 coinbaseTxHash, CAmount initial_coin) {
    sender = DecodeSecret(encoded_CKey);
    prevTxHash = coinbaseTxHash;
    sender_coin = initial_coin;
    receiver = generateKey();
}
void TemporaryContainer::generateTransaction(CTransactionRef& tx) {
    CMutableTransaction txNew;
    txNew.nLockTime = 0;    // set locktime as 0

    // txout build
    CTxDestination receiveDest = GetDestinationForKey(receiver.GetPubKey(), OutputType::P2SH_SEGWIT);
    CScript scriptReceive = GetScriptForDestination(receiveDest);
    CTxOut receive_prototype_txout(0, scriptReceive);
    size_t nSize = GetSerializeSize(receive_prototype_txout);
    CTxOut receive_txout(CFeeRate(DUST_RELAY_TX_FEE).GetFee(nSize) + 1, scriptReceive);
    txNew.vout.push_back(receive_txout);

    // dummy txchange build
    CKey changer = generateKey();
    CTxDestination changeDest = GetDestinationForKey(changer.GetPubKey(), OutputType::P2SH_SEGWIT);
    CScript scriptChange = GetScriptForDestination(changeDest);
    CTxOut change_prototype_txout(0, scriptChange);
    txNew.vout.push_back(change_prototype_txout);

    // dummy txinput build
    CTxIn dummy_txin(COutPoint(prevTx->GetHash(), 0), prevTx->vout[0].scriptPubKey, 0xffffffff);
    txNew.vin.push_back(dummy_txin);

    // sign
    SignatureData sigdata;
    std::vector<unsigned char> vchSig;
    uint256 hash = SignatureHash(prevTx->vout[0].scriptPubKey, &txNew, 0, SIGHASH_ALL, prevTx->vout[0].nValue, SigVersion::BASE);
    sender.Sign(hash, vchSig);
    vchSig.push_back((unsigned char)SIGHASH_ALL);
    sigdata.signatures.emplace(sender.GetPubKey().GetID(), SigPair(sender.GetPubKey(), vchSig));
    assert(sender.GetPubKey().Verify(hash, vchSig));




    SignatureData dummy_sigdata;
    ProduceSignature(*this, MutableTransactionSignatureCreator(&txNew, 0, prevTx->vout[0].nValue, SIGHASH_ALL), prevTx->vout[0].scriptPubKey, dummy_sigdata);
    UpdateInput(txNew.vin.at(0), dummy_sigdata);

    // fee calculation & txchange build
    CAmount nBytes = GetVirtualTransactionSize(CTransaction(txNew));
    CAmount nFeeNeeded = CFeeRate(DEFAULT_FALLBACK_FEE).GetFee(nBytes);
    CAmount change = prevTx->vout[0].nValue - nFeeNeeded - 1;
    CTxOut change_txout(change, scriptChange);
    txNew.vout.pop_back();
    txNew.vout.push_back(change_txout);

    // txinput build
//    SignatureData sigdata;
//    std::vector<unsigned char> vchSig;
//    MutableTransactionSignatureCreator signer(&txNew, 0, prevTx->vout[0].nValue, SIGHASH_ALL);
//    uint256 hash = SignatureHash(prevTx->vout[0].scriptPubKey, &txNew, 0, SIGHASH_ALL, prevTx->vout[0].nValue, SigVersion::BASE);
//    sender.Sign(hash, vchSig);
//    vchSig.push_back((unsigned char)SIGHASH_ALL);
//    sigdata.signatures.emplace(sender.GetPubKey().GetID(), SigPair(sender.GetPubKey(), vchSig));


    CTxIn txin(COutPoint(prevTx->GetHash(), 0), CScript(), 0);
    txNew.vin.pop_back();
    txNew.vin.push_back(txin);
    SignatureData sigdata;
    ProduceSignature(*this, MutableTransactionSignatureCreator(&txNew, 0, prevTx->vout[0].nValue, SIGHASH_ALL), prevTx->vout[0].scriptPubKey, sigdata);
    UpdateInput(txNew.vin.at(0), sigdata);

    sender = changer;
    prevTx = MakeTransactionRef(std::move(txNew));
}

int main(int argc, char* argv[]) {
//    BitcoinTxGen* b = new BitcoinTxGen("asdf");
    int initial_coin = atoi(argv[2]);
    TemporaryContainer t(argv[1], initial_coin);
    CTransactionRef tx;
    while(true) {
        for(int i=0; i<tx_per_period; i++) {
            t.generateTransaction(tx);
        }
        // send
        sleep(TXGEN_TIME_PER_PERIOD);
    }
    return 0;
}
