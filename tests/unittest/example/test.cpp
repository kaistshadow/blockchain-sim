#include "sum_integers.hpp"
#include "gtest/gtest.h"

#include <vector>

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  // ------------------------------------------------
  // RUN_ALL_TESTS 함수 실행을 통해, 아래에서 정의된 TEST() 함수들이 모두 실행이 됨.
  // ------------------------------------------------
  return RUN_ALL_TESTS();
}

TEST(example, sum_zero) {
  auto integers = {1, -1, 2, -2, 3, -3};
  auto result = sum_integers(integers);
  ASSERT_EQ(result, 0);
}

TEST(example, sum_five) {
  auto integers = {1, 2, 3, 4, 5};
  auto result = sum_integers(integers);
  ASSERT_EQ(result, 15);
}
