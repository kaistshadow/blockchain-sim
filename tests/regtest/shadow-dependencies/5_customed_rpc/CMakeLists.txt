cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake")
include(ShadowTools) # load ShadowTools Module

# find_package
find_package(OpenSSL_1_1_0h REQUIRED)
find_package(CURL_7_70_0 REQUIRED)
find_package(Bitcoin_0_19_1nw REQUIRED)
message(STATUS "OPENSSL_1_1_0H_FOUND=${OPENSSL_1_1_0H_FOUND}")
message(STATUS "CURL_7_70_0_FOUND=${CURL_7_70_0_FOUND}")
message(STATUS "BITCOIN_0_19_1NW_FOUND=${BITCOIN_0_19_1NW_FOUND}")
message(STATUS "BITCOIN_0_19_1NW_PLUGIN=${BITCOIN_0_19_1NW_PLUGIN}")

add_custom_target(bitcoin-dir-prepare ALL
        COMMAND rm -rf node1
        COMMAND mkdir node1
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1nw::Plugin> ${CMAKE_CURRENT_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_shadow_plugin(CUSTOM_RPC rpc.c)
target_link_libraries(CUSTOM_RPC OpenSSL_1_1_0h::Crypto OpenSSL_1_1_0h::SSL CURL_7_70_0::CURL)

# copy resulting plugins into source directory
add_custom_command(TARGET CUSTOM_RPC
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:CUSTOM_RPC> ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-bitcoin-custom-rpc
        COMMAND shadow example.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

