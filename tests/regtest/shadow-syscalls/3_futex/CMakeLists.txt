cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
enable_testing()

add_custom_target(future ALL
        COMMAND g++ -o future.so future.c -lpthread -shared -fPIC -g -std=c++11
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-futex-future
        COMMAND shadow example_future.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_tests_properties(shadow-futex-future PROPERTIES TIMEOUT 30)