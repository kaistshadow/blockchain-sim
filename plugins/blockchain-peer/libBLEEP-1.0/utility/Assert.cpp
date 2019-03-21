#include "Assert.h"

void libBLEEP::__M_Assert(const char* expr_str, bool expr, const char* file, int line, const char* msg) {
    if (!expr)
        {
            std::cout << "Assert failed:\t" << msg << "\n"
                      << "Expected:\t" << expr_str << "\n"
                      << "Source:\t\t" << file << ", line " << line << "\n";
            abort();
        }
}
