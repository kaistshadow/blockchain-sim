cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
enable_testing()

add_custom_target(glibc-pthread ALL
        COMMAND gcc -o pthread.so pthread.c -lpthread -shared -fPIC -g
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-pthread
        COMMAND shadow example_pthread.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})