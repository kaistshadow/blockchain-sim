//
// Created by csrc on 21. 3. 16..
//

#ifndef BLEEP_BITCOINTXGEN_H
#define BLEEP_BITCOINTXGEN_H

#include "BitcoinKey.h"
#include "BitcoinState.h"

class BitcoinTxgen {
public:
    unsigned char* generate(BitcoinState** state, BitcoinKey** key, size_t& streamsize);
};
#endif //BLEEP_BITCOINTXGEN_H