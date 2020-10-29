## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
if (NOT TARGET BLEEP) # check whether it's independent build
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake") # Path for BLEEP Module
    find_package(BLEEP REQUIRED) # load BLEEP Module
endif()
## enable CTEST
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake")

include(ShadowTools) # load ShadowTools Module

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

find_package(Libev_4_33 REQUIRED)

################################################## for simple-connect experiment
add_shadow_plugin(shadow-ISP-simple-connect-client test-client.cpp)
add_shadow_plugin(shadow-ISP-simple-connect-server test-server.cpp)
target_link_libraries(shadow-ISP-simple-connect-client
        Libev_4_33::Libev
        SHADOW_INTERFACE)
target_link_libraries(shadow-ISP-simple-connect-server
        Libev_4_33::Libev
        SHADOW_INTERFACE)


# copy resulting plugins into source directory
add_custom_command(TARGET shadow-ISP-simple-connect-client
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:shadow-ISP-simple-connect-client> ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_command(TARGET shadow-ISP-simple-connect-server
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:shadow-ISP-simple-connect-server> ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-ISP-simple-connect
        COMMAND shadow test-simple-connect.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_tests_properties(shadow-ISP-simple-connect PROPERTIES TIMEOUT 60)
