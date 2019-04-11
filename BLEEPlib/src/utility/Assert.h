#ifndef ASSERT_H
#define ASSERT_H

#include <iostream>

namespace libBLEEP {

#ifndef NDEBUG
#   define M_Assert(Expr, Msg)                          \
    __M_Assert(#Expr, Expr, __FILE__, __LINE__, Msg)
#else
#   define M_Assert(Expr, Msg) ;
#endif

    void __M_Assert(const char* expr_str, bool expr, const char* file, int line, const char* msg);

}
#endif
