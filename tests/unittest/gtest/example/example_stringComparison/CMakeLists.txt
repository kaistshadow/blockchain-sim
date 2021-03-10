###############################################
#   Step1. Naming test 
###############################################
set(This gtest-example_string)

###############################################
#   Step2. Compile and link executable file
###############################################
set(SRCS
        test.cpp)

add_library(string_ string_.cpp)
add_executable(${This} ${SRCS})
target_link_libraries(${This} PUBLIC 
        string_
        gtest_main
        )

install(TARGETS ${This} DESTINATION unittest)

###############################################
#   Step3. Set command comment to commend gtest
###############################################
enable_testing()

add_test(
    NAME string_google_test
    COMMAND $<TARGET_FILE:${This}>

)