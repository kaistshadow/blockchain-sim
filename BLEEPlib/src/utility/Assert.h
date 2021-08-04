// "Copyright [2021] <kaistshadow>"

#ifndef BLEEPLIB_SRC_UTILITY_ASSERT_H_
#define BLEEPLIB_SRC_UTILITY_ASSERT_H_

#include <iostream>

namespace libBLEEP {

#ifndef NDEBUG
#   define M_Assert(Expr, Msg)                          \
    __M_Assert(#Expr, Expr, __FILE__, __LINE__, Msg)
#else
    //Use same assert for the release mode
#   define M_Assert(Expr, Msg)                          \
    __M_Assert(#Expr, Expr, __FILE__, __LINE__, Msg)
#endif

    void __M_Assert(const char *expr_str, bool expr, const char *file, int line, const char *msg);

}  // namespace libBLEEP

#endif  // BLEEPLIB_SRC_UTILITY_ASSERT_H_
