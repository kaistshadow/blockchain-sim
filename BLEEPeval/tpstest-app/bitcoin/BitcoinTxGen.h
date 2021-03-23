//
// Created by csrc on 21. 3. 16..
//

#ifndef BLEEP_BITCOINTXGEN_H
#define BLEEP_BITCOINTXGEN_H

#include <key.h>
#include <primitives/transaction.h>

class BitcoinTxgen {
private:
    CKey secret;
    CTransaction* sourceTx;
public:
    BitcoinTxgen(const char* statefile, const char* keyfile);
    std::string generate();
};
#endif //BLEEP_BITCOINTXGEN_H