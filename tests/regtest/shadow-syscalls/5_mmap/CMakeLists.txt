cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
enable_testing()

add_custom_target(future ALL
        COMMAND gcc -o mmap.so mmap.c -lpthread -shared -fPIC -g -std=c++11
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-mmap
        COMMAND shadow test_mmap.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_tests_properties(shadow-mmap PROPERTIES TIMEOUT 30)