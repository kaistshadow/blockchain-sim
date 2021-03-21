//
// Created by csrc on 21. 3. 19..
//

#ifndef BLEEP_BITCOINSTATE_H
#define BLEEP_BITCOINSTATE_H

#include <primitives/transaction.h>

#include <stdio.h>

class BitcoinState {
private:
    CTransaction* sourceTx;
public:
    BitcoinState(const char* filename);
    ~BitcoinState();
};

#endif //BLEEP_BITCOINSTATE_H
