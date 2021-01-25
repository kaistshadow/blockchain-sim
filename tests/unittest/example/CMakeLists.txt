cmake_minimum_required(VERSION 3.5)

set(This gtest-example)

set(SRCS
        test.cpp)

add_library(sum_integers sum_integers.cpp)
add_executable(${This} ${SRCS})
target_link_libraries(${This} PUBLIC 
        sum_integers
        gtest_main
        )

install(TARGETS ${This} DESTINATION unittest)

enable_testing()

add_test(
    NAME google_test
    COMMAND $<TARGET_FILE:${This}>

)