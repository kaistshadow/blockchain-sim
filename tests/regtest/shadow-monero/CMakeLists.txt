## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)


## enable CTEST
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake")

#include(ShadowTools) # load ShadowTools Module
find_package(Monero_0.16.0 REQUIRED)

add_custom_target(shadow-monero-prepare ALL
        COMMAND mkdir -p data/bcdnode1
        COMMAND mkdir -p data/bcdnode2
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(copy-monero-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Monero_0.16.0::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-monero
        COMMAND shadow example.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


