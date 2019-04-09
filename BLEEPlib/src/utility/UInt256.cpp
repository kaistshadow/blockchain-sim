#include <iomanip>
#include <algorithm>
#include <string.h>
#include "UInt256.h"

using namespace libBLEEP;

std::string libBLEEP::UINT128_t::str() {
    unsigned char char_lower[8];
    memcpy(char_lower, &LOWER, 8);
    std::string str_lower((const char *)char_lower, 8);
    std::reverse(str_lower.begin(), str_lower.end());

    unsigned char char_upper[8];
    memcpy(char_upper, &UPPER, 8);
    std::string str_upper((const char *)char_upper, 8);
    std::reverse(str_upper.begin(), str_upper.end());

    std::string str = str_upper + str_lower;
    return str;
}

libBLEEP::UINT128_t::UINT128_t() 
    : UPPER(0), LOWER(0)
{}

libBLEEP::UINT128_t::UINT128_t(const UINT128_t & rhs) 
    : UPPER(rhs.UPPER), LOWER(rhs.LOWER)
{}

// Description about constructor endian.
// If buf "0011223344556677 8899aabbccddeeff" is given, (00 is at index 0, ff is at index 0xf)
// LOWER == 0x8899aabbccddeeff
// UPPER == 0x0011223344556677
// thus, resulting UINT128_t == 0x00112233445566778899aabbccddeeff
libBLEEP::UINT128_t::UINT128_t(const unsigned char rhs[], int len) {
    if (len <= 8) {
        UPPER = 0;
        LOWER = 0;
        for (int i = 0; i < len; i++) {
            LOWER <<= 8;
            LOWER |= (uint64_t)rhs[i];
        }
    }
    else {
        int lower_start_idx = len - 8;
        LOWER = 0;
        for (int i = lower_start_idx; i < len; i++) {
            LOWER <<= 8;
            LOWER |= (uint64_t)rhs[i];
        }

        UPPER = 0;
        for (int i = 0; i < lower_start_idx; i++) {
            UPPER <<=8;
            UPPER |= (uint64_t)rhs[i];
        }
    }
}


UINT128_t & libBLEEP::UINT128_t::operator=(const UINT128_t & rhs){
    UPPER = rhs.UPPER;
    LOWER = rhs.LOWER;
    return *this;
}

UINT128_t & libBLEEP::UINT128_t::operator=(UINT128_t && rhs){
    if (this != &rhs){
        UPPER = std::move(rhs.UPPER);
        LOWER = std::move(rhs.LOWER);
        rhs.UPPER = 0;
        rhs.LOWER = 0;
    }
    return *this;
}

UINT128_t libBLEEP::UINT128_t::operator&(const UINT128_t & rhs) const{
    return UINT128_t(UPPER & rhs.UPPER, LOWER & rhs.LOWER);
}

UINT128_t libBLEEP::UINT128_t::operator|(const UINT128_t & rhs) const{
    return UINT128_t(UPPER | rhs.UPPER, LOWER | rhs.LOWER);
}

UINT128_t libBLEEP::UINT128_t::operator^(const UINT128_t & rhs) const{
    return UINT128_t(UPPER ^ rhs.UPPER, LOWER ^ rhs.LOWER);
}

UINT128_t libBLEEP::UINT128_t::operator~() const{
    return UINT128_t(~UPPER, ~LOWER);
}

// Bit Shift Operators

UINT128_t libBLEEP::UINT128_t::operator<<(const UINT128_t & rhs) const{
    const uint64_t shift = rhs.LOWER;
    if (((bool) rhs.UPPER) || (shift >= 128)){
        return UINT128_t(0,0);
    }
    else if (shift == 64){
        return UINT128_t(LOWER, 0);
    }
    else if (shift == 0){
        return *this;
    }
    else if (shift < 64){
        return UINT128_t((UPPER << shift) + (LOWER >> (64 - shift)), LOWER << shift);
    }
    else if ((128 > shift) && (shift > 64)){
        return UINT128_t(LOWER << (shift - 64), 0);
    }
    else{
        return UINT128_t(0, 0);
    }
}

UINT128_t libBLEEP::UINT128_t::operator>>(const UINT128_t & rhs) const{
    const uint64_t shift = rhs.LOWER;
    if (((bool) rhs.UPPER) || (shift >= 128)){
        return UINT128_t(0, 0);
    }
    else if (shift == 64){
        return UINT128_t(0, UPPER);
    }
    else if (shift == 0){
        return *this;
    }
    else if (shift < 64){
        return UINT128_t(UPPER >> shift, (UPPER << (64 - shift)) + (LOWER >> shift));
    }
    else if ((128 > shift) && (shift > 64)){
        return UINT128_t(0, (UPPER >> (shift - 64)));
    }
    else{
        return UINT128_t(0,0);
    }
}

// Comparison operator
bool libBLEEP::UINT128_t::operator==(const UINT128_t & rhs) const{
    return ((UPPER == rhs.UPPER) && (LOWER == rhs.LOWER));
}

bool libBLEEP::UINT128_t::operator!=(const UINT128_t & rhs) const{
    return ((UPPER != rhs.UPPER) | (LOWER != rhs.LOWER));
}

bool libBLEEP::UINT128_t::operator>(const UINT128_t & rhs) const{
    if (UPPER == rhs.UPPER){
        return (LOWER > rhs.LOWER);
    }
    return (UPPER > rhs.UPPER);
}

bool libBLEEP::UINT128_t::operator<(const UINT128_t & rhs) const{
    if (UPPER == rhs.UPPER){
        return (LOWER < rhs.LOWER);
    }
    return (UPPER < rhs.UPPER);
}

bool libBLEEP::UINT128_t::operator>=(const UINT128_t & rhs) const{
    return ((*this > rhs) | (*this == rhs));
}

bool libBLEEP::UINT128_t::operator<=(const UINT128_t & rhs) const{
    return ((*this < rhs) | (*this == rhs));
}

// Arithmetic Operators
UINT128_t libBLEEP::UINT128_t::operator+(const UINT128_t & rhs) const{
    return UINT128_t(UPPER + rhs.UPPER + ((LOWER + rhs.LOWER) < LOWER), LOWER + rhs.LOWER);
}

UINT128_t libBLEEP::UINT128_t::operator-(const UINT128_t & rhs) const{
    return UINT128_t(UPPER - rhs.UPPER - ((LOWER - rhs.LOWER) > LOWER), LOWER - rhs.LOWER);
}

UINT128_t libBLEEP::UINT128_t::operator*(const UINT128_t & rhs) const{
    // split values into 4 32-bit parts
    uint64_t top[4] = {UPPER >> 32, UPPER & 0xffffffff, LOWER >> 32, LOWER & 0xffffffff};
    uint64_t bottom[4] = {rhs.UPPER >> 32, rhs.UPPER & 0xffffffff, rhs.LOWER >> 32, rhs.LOWER & 0xffffffff};
    uint64_t products[4][4];

    // multiply each component of the values
    for(int y = 3; y > -1; y--){
        for(int x = 3; x > -1; x--){
            products[3 - x][y] = top[x] * bottom[y];
        }
    }

    // first row
    uint64_t fourth32 = (products[0][3] & 0xffffffff);
    uint64_t third32  = (products[0][2] & 0xffffffff) + (products[0][3] >> 32);
    uint64_t second32 = (products[0][1] & 0xffffffff) + (products[0][2] >> 32);
    uint64_t first32  = (products[0][0] & 0xffffffff) + (products[0][1] >> 32);

    // second row
    third32  += (products[1][3] & 0xffffffff);
    second32 += (products[1][2] & 0xffffffff) + (products[1][3] >> 32);
    first32  += (products[1][1] & 0xffffffff) + (products[1][2] >> 32);

    // third row
    second32 += (products[2][3] & 0xffffffff);
    first32  += (products[2][2] & 0xffffffff) + (products[2][3] >> 32);

    // fourth row
    first32  += (products[3][3] & 0xffffffff);

    // move carry to next digit
    third32  += fourth32 >> 32;
    second32 += third32  >> 32;
    first32  += second32 >> 32;

    // remove carry from current digit
    fourth32 &= 0xffffffff;
    third32  &= 0xffffffff;
    second32 &= 0xffffffff;
    first32  &= 0xffffffff;

    // combine components
    return UINT128_t((first32 << 32) | second32, (third32 << 32) | fourth32);
}

std::string libBLEEP::UINT256_t::str() {
    std::string str = UPPER.str() + LOWER.str();
    return str;
}

libBLEEP::UINT256_t::UINT256_t() 
    : UPPER(0), LOWER(0)
{}

libBLEEP::UINT256_t::UINT256_t(const UINT256_t & rhs) 
    : UPPER(rhs.UPPER), LOWER(rhs.LOWER)
{}

// hash-friendly conversion. similar to UINT128_t
libBLEEP::UINT256_t::UINT256_t(const unsigned char rhs[], int len) {
    if (len > 16) {
        int lower_start = len - 16;
        LOWER = UINT128_t(rhs + lower_start, 16);
        UPPER = UINT128_t(rhs, lower_start);
    }
    else {
        UPPER = UINT128_t(0);
        LOWER = UINT128_t(rhs, len);
    }
}

UINT256_t libBLEEP::UINT256_t::operator&(const UINT128_t & rhs) const{
    return UINT256_t(UINT128_t(0), LOWER & rhs);
}

UINT256_t libBLEEP::UINT256_t::operator&(const UINT256_t & rhs) const{
    return UINT256_t(UPPER & rhs.UPPER, LOWER & rhs.LOWER);
}

UINT256_t libBLEEP::UINT256_t::operator|(const UINT128_t & rhs) const{
    return UINT256_t(UPPER , LOWER | rhs);
}

UINT256_t libBLEEP::UINT256_t::operator|(const UINT256_t & rhs) const{
    return UINT256_t(UPPER | rhs.UPPER, LOWER | rhs.LOWER);
}

UINT256_t libBLEEP::UINT256_t::operator^(const UINT128_t & rhs) const{
    return UINT256_t(UPPER, LOWER ^ rhs);
}

UINT256_t libBLEEP::UINT256_t::operator^(const UINT256_t & rhs) const{
    return UINT256_t(UPPER ^ rhs.UPPER, LOWER ^ rhs.LOWER);
}

UINT256_t libBLEEP::UINT256_t::operator~() const{
    return UINT256_t(~UPPER, ~LOWER);
}

UINT256_t libBLEEP::UINT256_t::operator<<(const UINT128_t & rhs) const{
    return *this << UINT256_t(rhs);
}

UINT256_t libBLEEP::UINT256_t::operator<<(const UINT256_t & rhs) const{
    const UINT128_t shift = rhs.LOWER;
    if ((rhs.UPPER > 0) || (shift >= UINT128_t(256))){
        return UINT256_t(0);
    }
    else if (shift == UINT128_t(128)){
        return UINT256_t(LOWER, UINT128_t(0));
    }
    else if (shift == UINT128_t(0)){
        return *this;
    }
    else if (shift < UINT128_t(128)){
        return UINT256_t((UPPER << shift) + (LOWER >> (UINT128_t(128) - shift)), LOWER << shift);
    }
    else if ((UINT128_t(256) > shift) && (shift > UINT128_t(128))){
        return UINT256_t(LOWER << (shift - UINT128_t(128)), UINT128_t(0));
    }
    else{
        return UINT256_t(0);
    }
}


UINT256_t libBLEEP::UINT256_t::operator>>(const UINT128_t & rhs) const{
    return *this >> UINT256_t(rhs);
}

UINT256_t libBLEEP::UINT256_t::operator>>(const UINT256_t & rhs) const{
    const UINT128_t shift = rhs.LOWER;
    if ((rhs.UPPER > 0) || (shift >= UINT128_t(256))){
        return UINT256_t(0);
    }
    else if (shift == UINT128_t(128)){
        return UINT256_t(UPPER);
    }
    else if (shift == UINT128_t(0)){
        return *this;
    }
    else if (shift < UINT128_t(128)){
        return UINT256_t(UPPER >> shift, (UPPER << (UINT128_t(128) - shift)) + (LOWER >> shift));
    }
    else if ((UINT128_t(256) > shift) && (shift > UINT128_t(128))){
        return UINT256_t(UPPER >> (shift - UINT128_t(128)));
    }
    else{
        return UINT256_t(0);
    }
}

bool libBLEEP::UINT256_t::operator==(const UINT128_t & rhs) const{
    return (*this == UINT256_t(rhs));
}

bool libBLEEP::UINT256_t::operator==(const UINT256_t & rhs) const{
    return ((UPPER == rhs.UPPER) && (LOWER == rhs.LOWER));
}


bool libBLEEP::UINT256_t::operator!=(const UINT128_t & rhs) const{
    return (*this != UINT256_t(rhs));
}

bool libBLEEP::UINT256_t::operator!=(const UINT256_t & rhs) const{
    return ((UPPER != rhs.UPPER) | (LOWER != rhs.LOWER));
}

bool libBLEEP::UINT256_t::operator>(const UINT128_t & rhs) const{
    return (*this > UINT256_t(rhs));
}

bool libBLEEP::UINT256_t::operator>(const UINT256_t & rhs) const{
    if (UPPER == rhs.UPPER){
        return (LOWER > rhs.LOWER);
    }
    if (UPPER > rhs.UPPER){
        return true;
    }
    return false;
}

bool libBLEEP::UINT256_t::operator<(const UINT128_t & rhs) const{
    return (*this < UINT256_t(rhs));
}

bool libBLEEP::UINT256_t::operator<(const UINT256_t & rhs) const{
    if (UPPER == rhs.UPPER){
        return (LOWER < rhs.LOWER);
    }
    if (UPPER < rhs.UPPER){
        return true;
    }
    return false;
}

bool libBLEEP::UINT256_t::operator>=(const UINT128_t & rhs) const{
    return (*this >= UINT256_t(rhs));
}

bool libBLEEP::UINT256_t::operator>=(const UINT256_t & rhs) const{
    return ((*this > rhs) | (*this == rhs));
}

bool libBLEEP::UINT256_t::operator<=(const UINT128_t & rhs) const{
    return (*this <= UINT256_t(rhs));
}

bool libBLEEP::UINT256_t::operator<=(const UINT256_t & rhs) const{
    return ((*this < rhs) | (*this == rhs));
}

UINT256_t libBLEEP::UINT256_t::operator+(const UINT128_t & rhs) const{
    return *this + UINT256_t(rhs);
}

UINT256_t libBLEEP::UINT256_t::operator+(const UINT256_t & rhs) const{
    return UINT256_t(UPPER + rhs.UPPER + (((LOWER + rhs.LOWER) < LOWER)?UINT128_t(1):UINT128_t(0)), LOWER + rhs.LOWER);
}

UINT256_t libBLEEP::UINT256_t::operator-(const UINT128_t & rhs) const{
    return *this - UINT256_t(rhs);
}

UINT256_t libBLEEP::UINT256_t::operator-(const UINT256_t & rhs) const{
    return UINT256_t(UPPER - rhs.UPPER - ((LOWER - rhs.LOWER) > LOWER), LOWER - rhs.LOWER);
}

UINT256_t libBLEEP::UINT256_t::operator*(const UINT128_t & rhs) const{
    return *this * UINT256_t(rhs);
}

UINT256_t libBLEEP::UINT256_t::operator*(const UINT256_t & rhs) const{
    // split values into 4 64-bit parts
    UINT128_t top[4] = {UPPER.upper(), UPPER.lower(), LOWER.upper(), LOWER.lower()};
    UINT128_t bottom[4] = {rhs.upper().upper(), rhs.upper().lower(), rhs.lower().upper(), rhs.lower().lower()};
    UINT128_t products[4][4];

    // multiply each component of the values
    for(int y = 3; y > -1; y--){
        for(int x = 3; x > -1; x--){
            products[3 - y][x] = top[x] * bottom[y];
        }
    }

    // first row
    UINT128_t fourth64 = UINT128_t(products[0][3].lower());
    UINT128_t third64  = UINT128_t(products[0][2].lower()) + UINT128_t(products[0][3].upper());
    UINT128_t second64 = UINT128_t(products[0][1].lower()) + UINT128_t(products[0][2].upper());
    UINT128_t first64  = UINT128_t(products[0][0].lower()) + UINT128_t(products[0][1].upper());

    // second row
    third64  = third64 + UINT128_t(products[1][3].lower());
    second64 = second64 + UINT128_t(products[1][2].lower()) + UINT128_t(products[1][3].upper());
    first64  = first64 + UINT128_t(products[1][1].lower()) + UINT128_t(products[1][2].upper());

    // third row
    second64 = second64 + UINT128_t(products[2][3].lower());
    first64  = first64 + UINT128_t(products[2][2].lower()) + UINT128_t(products[2][3].upper());

    // fourth row
    first64  = first64 + UINT128_t(products[3][3].lower());

    // combines the values, taking care of carry over
    return UINT256_t(first64 << UINT128_t(64), UINT128_t(0)) +
           UINT256_t(third64.upper(), third64 << UINT128_t(64)) +
           UINT256_t(second64, UINT128_t(0)) +
           UINT256_t(fourth64);
}

std::ostream & libBLEEP::operator<<(std::ostream & stream, const UINT128_t & rhs){
    std::ios oldState(nullptr);
    oldState.copyfmt(stream);
    
    stream << "[";
    stream << std::setfill('0') << std::setw(16) << std::hex << rhs.upper() << std::setfill('0') << std::setw(16) << rhs.lower();
    stream << "]";

    stream.copyfmt(oldState);

    return stream;
}

std::ostream & libBLEEP::operator<<(std::ostream & stream, const UINT256_t & rhs){
    std::ios oldState(nullptr);
    oldState.copyfmt(stream);

    stream << "[";
    stream << std::setfill('0') << std::setw(16) << std::hex << rhs.upper() << std::setfill('0') << std::setw(16) << rhs.lower();
    stream << "]";

    stream.copyfmt(oldState);
    return stream;
}
