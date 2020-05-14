## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
enable_testing()
## required for independent build


add_executable(HELLOCPP hello.cpp)

add_test(NAME BLEEP-regtest-native-bintest-hellocpp
        COMMAND HELLOCPP
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
