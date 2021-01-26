// ------------------------------------------------------------------
//                        참/거짓 단정문 샘플
// ------------------------------------------------------------------
#include "gtest/gtest.h"
#include <iostream>
#include "basic.hpp"

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
TEST(example_bool, bool_true) {  
  ASSERT_TRUE(return_True());
}

TEST(example_bool, bool_false) {
  ASSERT_FALSE(return_False());
}