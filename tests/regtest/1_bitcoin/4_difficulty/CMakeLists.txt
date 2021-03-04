## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

## enable CTEST
enable_testing()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake")
include(ShadowTools) # load ShadowTools Module


find_package(Bitcoin_0_19_1dev REQUIRED)

add_custom_target(shadow_bitcoin_difficulty_test_prepare ALL
  COMMAND rm -rf data/bcdnode0
  COMMAND mkdir -p data/bcdnode0
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(shadow_bitcoin_difficulty_test2 ALL
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(shadow_bitcoin_difficulty_rpc ALL
  COMMAND gcc -o rpc.so rpc.cpp ../libraries/rpc_client.cpp -ljsoncpp -lcurl -lssl -shared -fPIC -Wl,-rpath=${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/curl_7.70.0/lib -L${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/curl_7.70.0/lib -g -std=c99
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(shadow_bitcoin_difficulty_test2 Bitcoin_0_19_1dev::Plugin)
add_test(NAME shadow_bitcoin_difficulty_test
  COMMAND python3 difficulty.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)