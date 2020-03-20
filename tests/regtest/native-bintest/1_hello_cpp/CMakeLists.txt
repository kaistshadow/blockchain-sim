
add_executable(HELLOCPP hello.cpp)

add_test(NAME BLEEP-regtest-native-bintest-hellocpp
  COMMAND HELLOCPP)
