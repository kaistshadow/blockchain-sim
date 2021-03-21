//
// Created by csrc on 21. 3. 19..
//

#ifndef BLEEP_BITCOINKEY_H
#define BLEEP_BITCOINKEY_H

#include <key.h>

class BitcoinKey {
private:
    CKey secret;
public:
    BitcoinKey(const char* filename);
};

#endif //BLEEP_BITCOINKEY_H
