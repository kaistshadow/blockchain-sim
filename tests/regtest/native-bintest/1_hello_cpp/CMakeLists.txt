
add_executable(HELLOCPP hello.cpp)

add_test(NAME BLEEP-regtest-native-bintest-hellocpp
        COMMAND HELLOCPP
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
