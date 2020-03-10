cmake_minimum_required(VERSION 3.8)

set(This ExampleTests)

set(SRCS
        ExampleTests.cpp)

add_executable(${This} ${SRCS})
target_link_libraries(${This} PUBLIC
        gtest_main
        BLEEP)

install(TARGETS ${This} DESTINATION unittest)

add_test(
        NAME ${This}
        COMMAND ${CMAKE_INSTALL_PREFIX}/unittest/${This}
)
