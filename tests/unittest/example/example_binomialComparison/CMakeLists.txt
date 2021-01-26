cmake_minimum_required(VERSION 3.5)
###############################################
#   Step1. Naming test 
###############################################
set(This gtest-example-binomial)

###############################################
#   Step2. Compile and link executable file
###############################################
set(SRCS
        test.cpp)

add_library(sum_integers sum_integers.cpp)
add_executable(${This} ${SRCS})
target_link_libraries(${This} PUBLIC 
        sum_integers
        gtest_main
        )

install(TARGETS ${This} DESTINATION unittest)

###############################################
#   Step3. Set command comment to commend gtest
###############################################
enable_testing()

add_test(
    NAME binomial_google_test
    COMMAND $<TARGET_FILE:${This}>

)