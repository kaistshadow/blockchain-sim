cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
enable_testing()

add_custom_target(glibc-pthread_tls ALL
        COMMAND gcc -o libpthread.so pthread.c -lpthread -shared -fPIC -g
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-glibc-pthread_tls
        COMMAND shadow dtlstest.config.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})