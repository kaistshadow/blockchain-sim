## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

## enable CTEST
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake")
include(ShadowTools) # load ShadowTools Module



# find_package(OpenSSL 1.1.3 REQUIRED)
find_package(OpenSSL_1_1_0h REQUIRED)

message(STATUS "OPENSSL_1_1_0H_FOUND=${OPENSSL_1_1_0H_FOUND}")

add_shadow_plugin(sslrand ssl_rand.cpp)
target_link_libraries(sslrand OpenSSL_1_1_0h::Crypto OpenSSL_1_1_0h::SSL)

# copy resulting plugins into source directory
add_custom_command(TARGET sslrand
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sslrand> ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-custom-openssl-librand
        COMMAND shadow ssl_rand.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

