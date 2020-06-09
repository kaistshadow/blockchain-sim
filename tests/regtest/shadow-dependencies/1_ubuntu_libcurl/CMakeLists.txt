## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

add_custom_target(curl-ubuntu-bin ALL
        COMMAND gcc simplecurl.c -o simplecurl -lcurl
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(curl-ubuntu-shared ALL
        COMMAND gcc simplecurl.c -o libsimplecurl.so -lcurl -shared -fPIC
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-dependency-ubuntu-libcurl
        COMMAND shadow simplecurl.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

