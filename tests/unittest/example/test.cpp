// ------------------------------------------------------------------
#include "sum_integers.hpp"
#include "gtest/gtest.h"
#include <vector>

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
TEST(example, sum_zero) {
  auto integers = {1, -1, 2, -2, 3, -3};
  auto result = sum_integers(integers);
  ASSERT_EQ(result, 0); // If First parameter equal second parameter, return success.
}

TEST(example, sum_five) {
  auto integers = {1, 2, 3, 4, 5};
  auto result = sum_integers(integers);
  ASSERT_EQ(result, 15);
}
