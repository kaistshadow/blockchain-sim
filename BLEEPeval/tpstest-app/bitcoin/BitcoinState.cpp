//
// Created by csrc on 21. 3. 19..
//

#include "BitcoinState.h"


#include <config/bitcoin-config.h>
#include <clientversion.h>
#include <util/strencodings.h>
#include <serialize.h>
#include <primitives/transaction.h>
#include <streams.h>
#include <iostream>
#include <fstream>

// TODO
BitcoinState::BitcoinState(const char *filename) {
    std::ifstream ifs(filename);
    std::string txstr((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    CDataStream stream(ParseHex(txstr), SER_DISK, CLIENT_VERSION);
    CTransaction* sourceTx = new CTransaction(deserialize, stream);
    std::cout<<sourceTx->ToString()<<"\n";
}
// TODO
BitcoinState::~BitcoinState() {
    delete sourceTx;
}