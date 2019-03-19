#include <iomanip>
#include <sstream>
#include <iostream>
#include "hexstring.h"

// constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
//                            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

// std::string HexStr(unsigned char *data, int len)
// {
//   std::stringstream ss;
//   ss << std::hex;
//   for (int i = 0; i < len; ++i)
//     ss << std::setw(2) << std::setfill('0') << (int)data[i];
//   return ss.str();
// }

std::string utility::HexStr(std::string raw_input) 
{
    // convert to hex string
    std::string hexstring(raw_input.size() * 2, ' ');
    char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                     '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    for (int i = 0; i < raw_input.size(); i++) {
        hexstring[2 * i] = hexmap[(static_cast<int>(raw_input[i]) & 0xf0) >> 4];
        hexstring[2 * i + 1] = hexmap[static_cast<int>(raw_input[i]) & 0xf];
    }
    // std::cout << "in HexStr hexstring:" << hexstring << "\n";
    return hexstring;
}

// std::string hexStr(unsigned char *data, int len)
// {
//   std::string s(len * 2, ' ');
//   for (int i = 0; i < len; ++i) {
//     s[2 * i]     = hexmap[(data[i] & 0xF0) >> 4];
//     s[2 * i + 1] = hexmap[data[i] & 0x0F];
//   }

//   return s;
// }
