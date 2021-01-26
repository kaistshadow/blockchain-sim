// ------------------------------------------------------------------
//                        문자열 비교 샘플
// ------------------------------------------------------------------
#include "gtest/gtest.h"
#include <string>
#include "string.hpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
// -->  copy and write above codes

// ------------------------------------------------------------------
// Define test name
// TEST function param1. main test name
// TEST function param2. sub test name
// ------------------------------------------------------------------
TEST(example_string, stringComparison1) {
  const char* s3 = "hello";
  const char* s4 = string_comparison(); // 모듈 테스트.
  ASSERT_STREQ(s3,s4); // 문자열 비교
}