## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)


## enable CTEST
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake")

#include(ShadowTools) # load ShadowTools Module
add_custom_target(copy-litecoin-plugin-wallet ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:LITECOIN__DEV_::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(copy-litecoin-plugin-wallet LITECOIN__DEV_::Plugin)

add_custom_target(shadow-litecoin-wallet ALL
  COMMAND gcc -o rpc.so rpc.cpp ../../../../testlibs/rpc_client.cpp -ljsoncpp -lcurl -lssl -shared -fPIC -Wl,-rpath=${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/curl_7.70.0/lib -L${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/curl_7.70.0/lib -g -std=c99
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-litecoin-wallet-test
        COMMAND shadow example.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

