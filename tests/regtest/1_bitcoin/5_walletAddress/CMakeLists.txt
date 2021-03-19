## required for independent build
cmake_minimum_required(VERSION 2.8...3.13)

## enable CTEST
enable_testing()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake")
include(ShadowTools) # load ShadowTools Module


find_package(Bitcoin_0_19_1dev REQUIRED)

add_custom_target(bitcoinWallet_shadow_test_prepare ALL
  COMMAND rm -rf data/bcdnode0
  COMMAND mkdir -p data/bcdnode0
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(bitcoinWallet_shadow_test_prepare2 ALL
        DEPENDS Bitcoin_0_19_1dev::Plugin
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpc.so
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpc.so
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../../../testlibs/rpc_client.cpp
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpc.cpp
        COMMAND gcc -o rpc.so rpc.cpp ../../../../testlibs/rpc_client.cpp -ljsoncpp -lcurl -lssl -shared -fPIC -Wl,-rpath=${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/curl_7.70.0/lib -L${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/curl_7.70.0/lib -g -std=c99
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME bitcoinWallet_shadow_test
  COMMAND python3 walletAddress.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
set_property(TEST bitcoinWallet_shadow_test APPEND
        PROPERTY
        ENVIRONMENT PATH=$ENV{PATH}:$<TARGET_FILE_DIR:shadow>) # add path environment in order to run this test on CLion