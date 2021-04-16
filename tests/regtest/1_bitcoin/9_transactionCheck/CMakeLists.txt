## required for independent build
cmake_minimum_required(VERSION 2.8...3.13)

## enable CTEST
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake")
include(ShadowTools) # load ShadowTools Module


find_package(Bitcoin_0_19_1dev REQUIRED)

add_custom_target(shadow-bitcoin-transaction_count-copy-plugin ALL
        DEPENDS Bitcoin_0_19_1dev::Plugin
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpc.so
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/transaction.so
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpc.so
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/transaction.so
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/one_node_setmine.cpp
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../../../testlibs/rpc_client.cpp
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/transaction.cpp
        COMMAND gcc -o rpc.so one_node_setmine.cpp ../../../../testlibs/rpc_client.cpp -ljsoncpp -lcurl -lssl -shared -fPIC -Wl,-rpath=${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/curl_7.70.0/lib -L${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/curl_7.70.0/lib -g -std=c99
        COMMAND gcc -o transaction.so transaction.cpp ../../../../testlibs/rpc_client.cpp -ljsoncpp -lcurl -lssl -shared -fPIC -Wl,-rpath=${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/curl_7.70.0/lib -L${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/curl_7.70.0/lib -g
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-bitcoin-transaction_count-test
COMMAND python3 transaction_count.py
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set_property(TEST shadow-bitcoin-transaction_count-test APPEND
        PROPERTY
        ENVIRONMENT PATH=$ENV{PATH}:$<TARGET_FILE_DIR:shadow>) # add path environment in order to run this test on CLion