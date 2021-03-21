//
// Created by csrc on 21. 3. 19..
//

#include "BitcoinKey.h"

#include <key_io.h>

#include <iostream>
#include <fstream>

BitcoinKey::BitcoinKey(const char *filename) {
    std::ifstream ifs(filename);
    std::string keystr((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    secret = DecodeSecret(keystr);
}