## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

## enable CTEST
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake")
include(ShadowTools) # load ShadowTools Module


find_package(Bitcoin_0_19_1dev REQUIRED)

add_custom_target(shadow-bitcoin-dev-test-prepare ALL
  COMMAND rm -rf data/bcdnode0
  COMMAND rm -rf data/bcdnode1
  COMMAND mkdir -p data/bcdnode0
  COMMAND mkdir -p data/bcdnode1
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# TODO : unresolved dependency problem during full build is still remaining.
add_custom_target(shadow-bitcoin-dev-test-copy-plugin ALL
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(shadow-bitcoin-dev-test-copy-plugin Bitcoin_0_19_1dev::Plugin)

add_custom_target(shadow-bitcoin-dev-one-node-mine-plugin ALL
        COMMAND gcc -o one_node_setmine.so one_node_setmine.c -lcurl -lssl -shared -fPIC -Wl,-rpath=${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/curl_7.70.0/lib -L${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/curl_7.70.0/lib -g -std=c99
        COMMAND gcc -o transaction.so transaction.c -lcurl -lssl -shared -fPIC -Wl,-rpath=${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/curl_7.70.0/lib -L${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/curl_7.70.0/lib -g
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-bitcoin-dev-test
        COMMAND shadow example.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_tests_properties(shadow-bitcoin-dev-test PROPERTIES TIMEOUT 60)
