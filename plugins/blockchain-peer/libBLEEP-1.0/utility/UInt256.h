#ifndef UTILITY_UINT256_H
#define UTILITY_UINT256_H

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <string>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

// Almost code in this file are from https://github.com/calccrypto/UINT256_t 

namespace libBLEEP {
    class UINT128_t{
    private:
        uint64_t UPPER, LOWER;

        friend class boost::serialization::access;
        // When the class Archive corresponds to an output archive, the
        // & operator is defined similar to <<.  Likewise, when the class Archive
        // is a type of input archive the & operator is defined similar to >>
    
        template<class Archive>
            void serialize(Archive & ar, const unsigned int version) {
            ar & UPPER;
            ar & LOWER;
        }

    public:
        std::string str();

        const uint64_t & upper() const { return UPPER; }
        const uint64_t & lower() const { return LOWER; }

        // Constructors
        UINT128_t();
        UINT128_t(const UINT128_t & rhs);

        UINT128_t(const unsigned char rhs[], int len);

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT128_t(const T & rhs)
            : UPPER(0), LOWER(rhs)
        {}

        template <typename S, typename T, typename = typename std::enable_if <std::is_integral<S>::value && std::is_integral<T>::value, void>::type>
        UINT128_t(const S & upper_rhs, const T & lower_rhs)
            : UPPER(upper_rhs), LOWER(lower_rhs)
        {}       

        // Assignment Operator
        UINT128_t & operator=(const UINT128_t & rhs);
        UINT128_t & operator=(UINT128_t && rhs);

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT128_t & operator=(const T & rhs){
            UPPER = 0;
            LOWER = rhs;
            return *this;
        }

        // Bitwise Operator
        UINT128_t operator&(const UINT128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT128_t operator&(const T & rhs) const{
            return UINT128_t(0, LOWER & (uint64_t) rhs);
        }

        UINT128_t operator|(const UINT128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT128_t operator|(const T & rhs) const{
            return UINT128_t(UPPER, LOWER | (uint64_t) rhs);
        }

        UINT128_t operator^(const UINT128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT128_t operator^(const T & rhs) const{
            return UINT128_t(UPPER, LOWER ^ (uint64_t) rhs);
        }

        UINT128_t operator~() const;

        // Bit Shift Operators
        UINT128_t operator<<(const UINT128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT128_t operator<<(const T & rhs) const{
            return *this << UINT128_t(rhs);
        }

        UINT128_t operator>>(const UINT128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT128_t operator>>(const T & rhs) const{
            return *this >> UINT128_t(rhs);
        }

        // Comparison Operators
        bool operator==(const UINT128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator==(const T & rhs) const{
            return (!UPPER && (LOWER == (uint64_t) rhs));
        }

        bool operator!=(const UINT128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator!=(const T & rhs) const{
            return (UPPER | (LOWER != (uint64_t) rhs));
        }

        bool operator>(const UINT128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator>(const T & rhs) const{
            return (UPPER || (LOWER > (uint64_t) rhs));
        }

        bool operator<(const UINT128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator<(const T & rhs) const{
            return (!UPPER)?(LOWER < (uint64_t) rhs):false;
        }

        bool operator>=(const UINT128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator>=(const T & rhs) const{
            return ((*this > rhs) | (*this == rhs));
        }

        bool operator<=(const UINT128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator<=(const T & rhs) const{
            return ((*this < rhs) | (*this == rhs));
        }


        // Arithmetic Operators
        UINT128_t operator+(const UINT128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT128_t operator+(const T & rhs) const{
            return UINT128_t(UPPER + ((LOWER + (uint64_t) rhs) < LOWER), LOWER + (uint64_t) rhs);
        }

        UINT128_t operator-(const UINT128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT128_t operator-(const T & rhs) const{
            return UINT128_t((uint64_t) (UPPER - ((LOWER - rhs) > LOWER)), (uint64_t) (LOWER - rhs));
        }

        UINT128_t operator*(const UINT128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT128_t operator*(const T & rhs) const{
            return *this * UINT128_t(rhs);
        }
    };

    
    class UINT256_t{
    private:
        UINT128_t UPPER, LOWER;
        
        friend class boost::serialization::access;
        // When the class Archive corresponds to an output archive, the
        // & operator is defined similar to <<.  Likewise, when the class Archive
        // is a type of input archive the & operator is defined similar to >>
    
        template<class Archive>
            void serialize(Archive & ar, const unsigned int version) {
            ar & UPPER;
            ar & LOWER;
        }

    public:
        const UINT128_t & upper() const { return UPPER; }
        const UINT128_t & lower() const { return LOWER; }
        std::string str();

        UINT256_t();
        UINT256_t(const UINT256_t & rhs);
        UINT256_t(const UINT128_t & rhs)
            : UPPER(0), LOWER(rhs)
        {}

        UINT256_t(const UINT128_t & upper_rhs, const UINT128_t & lower_rhs)
            : UPPER(upper_rhs), LOWER(lower_rhs)
        {}

        UINT256_t(const unsigned char rhs[], int len);

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT256_t(const T & rhs)
            : UPPER(0,0), LOWER(rhs)
        {}

        template <typename S, typename T, typename = typename std::enable_if <std::is_integral<S>::value && std::is_integral<T>::value, void>::type>
        UINT256_t(const S & upper_rhs, const T & lower_rhs)
            : UPPER(upper_rhs), LOWER(lower_rhs)
        {}

        template <typename R, typename S, typename T, typename U,
                typename = typename std::enable_if<std::is_integral<R>::value &&
                std::is_integral<S>::value &&
                std::is_integral<T>::value &&
                std::is_integral<U>::value, void>::type>
        UINT256_t(const R & upper_lhs, const S & lower_lhs, const T & upper_rhs, const U & lower_rhs)
            : UPPER(upper_lhs, lower_lhs), LOWER(upper_rhs, lower_rhs)
        {}

        // Bitwise Operators
        UINT256_t operator&(const UINT128_t & rhs) const;
        UINT256_t operator&(const UINT256_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT256_t operator&(const T & rhs) const{
            return UINT256_t(UINT128_t(0), LOWER & (UINT128_t) rhs);
        }

        UINT256_t operator|(const UINT128_t & rhs) const;
        UINT256_t operator|(const UINT256_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT256_t operator|(const T & rhs) const{
            return UINT256_t(UPPER, LOWER | UINT128_t(rhs));
        }

        UINT256_t operator^(const UINT128_t & rhs) const;
        UINT256_t operator^(const UINT256_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT256_t operator^(const T & rhs) const{
            return UINT256_t(UPPER, LOWER ^ (UINT128_t) rhs);
        }

        UINT256_t operator~() const;

        // Bit Shift Operators
        UINT256_t operator<<(const UINT128_t & shift) const;
        UINT256_t operator<<(const UINT256_t & shift) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT256_t operator<<(const T & rhs) const{
            return *this << UINT256_t(rhs);
        }

        UINT256_t operator>>(const UINT128_t & shift) const;
        UINT256_t operator>>(const UINT256_t & shift) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT256_t operator>>(const T & rhs) const{
            return *this >> UINT256_t(rhs);
        }

        // Comparison Operators
        bool operator==(const UINT128_t & rhs) const;
        bool operator==(const UINT256_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator==(const T & rhs) const{
            return ( (UPPER == 0) && (LOWER == UINT128_t(rhs)));
        }

        bool operator!=(const UINT128_t & rhs) const;
        bool operator!=(const UINT256_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator!=(const T & rhs) const{
            return ((UPPER > 0) | (LOWER != UINT128_t(rhs)));
        }

        bool operator>(const UINT128_t & rhs) const;
        bool operator>(const UINT256_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator>(const T & rhs) const{
            return ((UPPER > 0) | (LOWER > UINT128_t(rhs)));
        }

        bool operator<(const UINT128_t & rhs) const;
        bool operator<(const UINT256_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator<(const T & rhs) const{
            return (UPPER == 0)?(LOWER < UINT128_t(rhs)):false;
        }

        bool operator>=(const UINT128_t & rhs) const;
        bool operator>=(const UINT256_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator>=(const T & rhs) const{
            return ((*this > rhs) | (*this == rhs));
        }

        bool operator<=(const UINT128_t & rhs) const;
        bool operator<=(const UINT256_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator<=(const T & rhs) const{
            return ((*this < rhs) | (*this == rhs));
        }
        
        // Arithmetic Operators
        UINT256_t operator+(const UINT128_t & rhs) const;
        UINT256_t operator+(const UINT256_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT256_t operator+(const T & rhs) const{
            return UINT256_t(UPPER + ((LOWER + (UINT128_t) rhs) < LOWER), LOWER + (UINT128_t) rhs);
        }

        UINT256_t operator-(const UINT128_t & rhs) const;
        UINT256_t operator-(const UINT256_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT256_t operator-(const T & rhs) const{
            return UINT256_t(UPPER - ((LOWER - rhs) > LOWER), LOWER - rhs);
        }

        UINT256_t operator*(const UINT128_t & rhs) const;
        UINT256_t operator*(const UINT256_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        UINT256_t operator*(const T & rhs) const{
            return *this * UINT256_t(rhs);
        }
    };


    // IO Operator
    std::ostream & operator<<(std::ostream & stream, const UINT128_t & rhs);
    std::ostream & operator<<(std::ostream & stream, const UINT256_t & rhs);
}

#endif
